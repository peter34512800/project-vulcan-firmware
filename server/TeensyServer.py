import socket
import csv
import os
import sys

def get_next_available_filename():
    filename = "data.txt"
    python_dir = os.path.dirname(os.path.abspath(__file__))
    if not os.path.isfile(os.path.join(python_dir, filename)):
        return filename

    index = 1

    while os.path.isfile(os.path.join(python_dir, f"data{index}.txt")):
        index += 1

    return f"data{index}.txt"




def start_tcp_server(host="0.0.0.0", port=5002, output_filename=None):
    if output_filename is None:
        output_filename = get_next_available_filename()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as serverSocket:
        serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        serverSocket.bind((host, port))

        serverSocket.listen(1)
        print(f"Server listening on {host}:{port}")


        while True:
            clientSocket, clientAddress = serverSocket.accept()
            print(f"Connection accepted from {clientAddress}")

            clientSocket.settimeout(5.0)

            file_mode = "ab"
            log_file = None
            buffer = b""

            with clientSocket:
                while True:
                    try:
                        data = clientSocket.recv(2048)
                        if not data:
                            print(f"Teensy disconnected: {clientAddress}")
                            break
                        buffer += data

                        while b"\n" in buffer:
                            line, buffer = buffer.split(b"\n", 1)
                            line_str = line.decode("utf-8", errors="ignore").strip()

                            if line_str.startswith("MODE:"):
                                print(f"Header {line_str}")

                                if "MODE:SERIAL_PRINT" in line_str:
                                    # Just a header announcing a print is coming; nothing to do yet
                                    continue
                                elif "MODE:DATA_SYNC" in line_str:
                                    new_mode = "wb"
                                elif "MODE:DATA_LOG" in line_str:
                                    new_mode = "ab"
                                else:
                                    new_mode = "ab"
                                    print("UNKNOWN HEADER: defaulting to append")

                                if log_file is None or new_mode != file_mode:
                                    if log_file:
                                        log_file.close()
                                    file_mode = new_mode
                                    log_file = open(output_filename, file_mode)
                                continue

                            if line_str.startswith("PRINT:"):
                                
                                print(f"[TEENSY] {line_str[len('PRINT:'):]}")
                                continue

                            
                            if log_file is None:
                                log_file = open(output_filename, file_mode)
                            log_file.write((line_str + "\n").encode())
                            log_file.flush()
                            print(line_str)

                    except socket.timeout:
                        print("No data received for 5 second")
                        continue

                    except ConnectionResetError:
                        print(f"Teensy connection reset: {clientAddress}")
                        break
            if log_file:
                log_file.close()

if __name__ == "__main__":
    start_tcp_server()

