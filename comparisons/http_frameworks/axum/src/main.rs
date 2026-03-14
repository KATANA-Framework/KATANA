use axum::{
    extract::Json,
    http::StatusCode,
    routing::{get, post},
    Router,
};
use hyper::server::conn::http1;
use hyper_util::{rt::TokioIo, service::TowerToHyperService};
use std::env;
use std::net::{Ipv4Addr, SocketAddr};

const DEFAULT_PORT: u16 = 8080;
const DEFAULT_WORKERS: usize = 4;
const MAX_ITEMS: usize = 1024;

fn read_port() -> u16 {
    for key in ["PORT", "HELLO_PORT", "COMPUTE_PORT"] {
        if let Ok(value) = env::var(key) {
            if let Ok(port) = value.parse::<u16>() {
                if port > 0 {
                    return port;
                }
            }
        }
    }
    DEFAULT_PORT
}

fn read_workers() -> usize {
    for key in ["BENCH_WORKERS", "KATANA_WORKERS", "WORKERS"] {
        if let Ok(value) = env::var(key) {
            if let Ok(workers) = value.parse::<usize>() {
                if workers > 0 {
                    return workers;
                }
            }
        }
    }
    DEFAULT_WORKERS
}

async fn hello() -> &'static str {
    "Hello, World!"
}

async fn compute_sum(
    Json(nums): Json<Vec<f64>>,
) -> Result<Json<f64>, (StatusCode, &'static str)> {
    if nums.is_empty() || nums.len() > MAX_ITEMS {
        return Err((StatusCode::BAD_REQUEST, "payload must contain 1..=1024 numbers"));
    }

    Ok(Json(nums.into_iter().sum()))
}

fn main() {
    let port = read_port();
    let workers = read_workers();
    let runtime = tokio::runtime::Builder::new_multi_thread()
        .worker_threads(workers)
        .enable_all()
        .build()
        .expect("failed to build tokio runtime");

    runtime.block_on(async move {
        let app = Router::new()
            .route("/", get(hello))
            .route("/compute/sum", post(compute_sum));

        let addr = SocketAddr::from((Ipv4Addr::UNSPECIFIED, port));
        let listener = tokio::net::TcpListener::bind(addr)
            .await
            .expect("failed to bind listener");

        println!("axum comparison server listening on :{port} with {workers} workers");

        loop {
            let (socket, _peer_addr) = listener.accept().await.expect("failed to accept connection");
            socket
                .set_nodelay(true)
                .expect("failed to enable TCP_NODELAY");

            let service = TowerToHyperService::new(app.clone());
            tokio::spawn(async move {
                let io = TokioIo::new(socket);
                let mut builder = http1::Builder::new();
                builder.keep_alive(true);
                builder.pipeline_flush(true);

                if let Err(error) = builder.serve_connection(io, service).await {
                    eprintln!("axum connection error: {error}");
                }
            });
        }
    });
}
