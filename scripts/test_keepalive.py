import socket


def read_until(sock, marker, timeout=2.0):
    sock.settimeout(timeout)
    data = b""
    while marker not in data:
        chunk = sock.recv(4096)
        if not chunk:
            return None
        data += chunk
    return data


def parse_content_length(headers_blob):
    for line in headers_blob.split(b"\r\n"):
        if line.lower().startswith(b"content-length:"):
            try:
                return int(line.split(b":", 1)[1].strip())
            except ValueError:
                return None
    return None


def make_request(host="127.0.0.1", port=8080, count=5):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
    sock.settimeout(2.0)
    sock.connect((host, port))

    req = (
        b"POST /compute/sum HTTP/1.1\r\n"
        b"Host: 127.0.0.1:8080\r\n"
        b"Connection: keep-alive\r\n"
        b"Content-Type: application/json\r\n"
        b"Content-Length: 7\r\n"
        b"\r\n"
        b"[1,2,3]"
    )

    for i in range(count):
        stage = "send"
        try:
            sock.sendall(req)
            stage = "read_headers"
            data = read_until(sock, b"\r\n\r\n")
            if data is None:
                print(f"Request {i + 1}: Connection closed by server!")
                return False

            header_blob, rest = data.split(b"\r\n\r\n", 1)
            header_lines = header_blob.split(b"\r\n")
            status_line = header_lines[0] if header_lines else b""

            content_length = parse_content_length(header_blob)
            if content_length is None:
                print(f"Request {i + 1}: Missing Content-Length")
                return False

            stage = "read_body"
            while len(rest) < content_length:
                chunk = sock.recv(4096)
                if not chunk:
                    print(f"Request {i + 1}: Connection closed mid-body")
                    return False
                rest += chunk

            body = rest[:content_length]
            print(f"Request {i + 1}: {status_line.decode()} - body length: {len(body)}")
        except Exception as exc:
            print(f"Request {i + 1}: ERROR during {stage} - {exc}")
            return False

    sock.close()
    print("All requests succeeded on same socket!")
    return True


if __name__ == "__main__":
    make_request()
