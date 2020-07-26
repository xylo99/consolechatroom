FROM alpine:3.7
RUN apk add --update --no-cache build-base lsof
RUN mkdir -p /opt/bin
RUN mkdir -p /opt/src
COPY server.c /opt/src/
RUN gcc /opt/src/server.c -o /opt/bin/server
ENTRYPOINT ["/opt/bin/server", "&"]
