use ntex::web::{self, App, HttpResponse, HttpServer};
use std::env;
use std::io;

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

async fn hello() -> HttpResponse {
    HttpResponse::Ok()
        .content_type("text/plain; charset=utf-8")
        .body("Hello, World!")
}

async fn compute_sum(nums: web::types::Json<Vec<f64>>) -> HttpResponse {
    let values = nums.into_inner();
    if values.is_empty() || values.len() > MAX_ITEMS {
        return HttpResponse::BadRequest()
            .content_type("text/plain; charset=utf-8")
            .body("payload must contain 1..=1024 numbers");
    }

    let total: f64 = values.into_iter().sum();
    HttpResponse::Ok().json(&total)
}

#[ntex::main]
async fn main() -> io::Result<()> {
    let port = read_port();
    let workers = read_workers();

    println!("ntex comparison server listening on :{port} with {workers} workers");

    HttpServer::new(|| {
        App::new()
            .service(web::resource("/").route(web::get().to(hello)))
            .service(web::resource("/compute/sum").route(web::post().to(compute_sum)))
    })
    .workers(workers)
    .bind(("0.0.0.0", port))?
    .run()
    .await
}
