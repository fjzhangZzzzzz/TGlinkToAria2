FROM python:3.9-alpine
LABEL org.opencontainers.image.source="https://github.com/fjzhangzzzzzz/tglink2aria2"

WORKDIR /app

COPY requirements.txt ./

RUN apk add build-base

RUN pip install --no-cache-dir -r requirements.txt

COPY . .

CMD ["python3","-m","WebStreamer"]
