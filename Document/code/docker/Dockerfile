FROM python:3.11-slim

WORKDIR /app

COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

COPY proto ./proto
COPY src ./src

ENV PYTHONPATH=/app/src

RUN python -m grpc_tools.protoc -I ./proto \
    --python_out=./src \
    --grpc_python_out=./src \
    ./proto/procesos.proto

EXPOSE 50051

CMD ["python", "src/process.py"]
