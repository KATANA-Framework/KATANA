import argparse
import json
import random
import socket
import threading
import time
from collections import Counter


def build_request(host, port, payload):
    headers = [
        f"POST /compute/sum HTTP/1.1",
        f"Host: {host}:{port}",
        "Connection: keep-alive",
        "Content-Type: application/json",
        "Accept: application/json",
        f"Content-Length: {len(payload)}",
        "",
        "",
    ]
    return ("\r\n".join(headers).encode("utf-8") + payload)


def read_response(sock):
    data = b""
    while b"\r\n\r\n" not in data:
        chunk = sock.recv(4096)
        if not chunk:
            return None, None
        data += chunk
    header, rest = data.split(b"\r\n\r\n", 1)
    lines = header.split(b"\r\n")
    if not lines:
        return None, None
    parts = lines[0].split()
    if len(parts) < 2:
        return None, None
    try:
        status = int(parts[1])
    except Exception:
        return None, None
    content_length = 0
    for line in lines[1:]:
        if line.lower().startswith(b"content-length:"):
            try:
                content_length = int(line.split(b":", 1)[1].strip())
            except Exception:
                content_length = 0
    to_read = max(content_length - len(rest), 0)
    while to_read > 0:
        chunk = sock.recv(to_read)
        if not chunk:
            break
        rest += chunk
        to_read -= len(chunk)
    return status, rest


def make_socket(host, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    s.settimeout(5.0)
    s.connect((host, port))
    return s


def run(thread_count, duration_s, close_after):
    host, port = "127.0.0.1", 8080
    sizes = [1, 8, 64, 256, 1024]
    payloads = {
        size: json.dumps([float(i + 1) for i in range(size)]).encode("utf-8")
        for size in sizes
    }
    request_cache = {size: build_request(host, port, payload) for size, payload in payloads.items()}

    totals = Counter()
    errors = Counter()
    status_hist = Counter()
    exc_types = Counter()
    stop_event = threading.Event()

    connections_per_thread = 1 if close_after else max(4, min(8, thread_count * 2))

    def worker():
        rng = random.Random()
        socks = []
        for _ in range(connections_per_thread):
            try:
                socks.append(make_socket(host, port))
            except Exception as exc:
                exc_types[f"connect:{type(exc).__name__}"] += 1

        idx = 0
        while not stop_event.is_set():
            size = sizes[rng.randrange(len(sizes))]
            req_bytes = request_cache[size]
            sock = None

            if socks:
                sock = socks[idx % len(socks)]
                idx += 1

            try:
                if not sock:
                    sock = make_socket(host, port)
                    if not close_after:
                        socks.append(sock)

                sock.sendall(req_bytes)
                status, _ = read_response(sock)

                totals[size] += 1
                if status is not None:
                    status_hist[f"{size}:{status}"] += 1
                else:
                    status_hist[f"{size}:0"] += 1

                if status != 200:
                    errors[size] += 1
                    if sock:
                        sock.close()
                        if sock in socks:
                            socks.remove(sock)
                        sock = None
                elif close_after and sock:
                    sock.close()
                    sock = None

            except Exception as exc:
                errors[size] += 1
                if isinstance(exc, OSError) and exc.errno is not None:
                    exc_types[f"recv:{exc.errno}"] += 1
                else:
                    exc_types[f"recv:{type(exc).__name__}"] += 1
                try:
                    if sock and sock in socks:
                        sock.close()
                        socks.remove(sock)
                except Exception:
                    pass

        for s in socks:
            try:
                s.close()
            except Exception:
                pass

    threads = [threading.Thread(target=worker) for _ in range(thread_count)]
    for t in threads:
        t.start()
    time.sleep(duration_s)
    stop_event.set()
    for t in threads:
        t.join()

    print(
        f"threads={thread_count} duration={duration_s}s connections_per_thread={connections_per_thread} close_after={close_after}"
    )
    print(f"total_requests={sum(totals.values())} total_errors={sum(errors.values())}")
    print("status_0_by_size:")
    for size in sizes:
        print(f"  size={size}: {status_hist.get(f'{size}:0', 0)}")
    status_totals = Counter()
    for key, count in status_hist.items():
        _, status = key.split(":", 1)
        status_totals[status] += count
    print("status_totals:")
    for status, count in status_totals.most_common():
        print(f"  status_{status}: {count}")
    print("exceptions:")
    for key, count in exc_types.most_common():
        print(f"  {key}: {count}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--threads", type=int, default=16)
    parser.add_argument("--duration", type=int, default=10)
    parser.add_argument("--close", action="store_true")
    args = parser.parse_args()
    run(args.threads, args.duration, args.close)


if __name__ == "__main__":
    main()
