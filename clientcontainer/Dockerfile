FROM alpine:3.7
RUN apk add --update --no-cache build-base
RUN mkdir -p /opt/src
RUN mkdir -p /opt/bin
COPY client.c /opt/src/
RUN gcc /opt/src/client.c -o /opt/bin/client

