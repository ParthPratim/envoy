load(
    "//bazel:envoy_build_system.bzl",
    "envoy_package",
)

licenses(["notice"])  # Apache 2

envoy_package()

filegroup(
    name = "configs",
    srcs = [
        "cache/front-envoy.yaml",
        "cache/service-envoy.yaml",
        "cors/backend/front-envoy.yaml",
        "cors/backend/service-envoy.yaml",
        "cors/frontend/front-envoy.yaml",
        "cors/frontend/service-envoy.yaml",
        "csrf/crosssite/front-envoy.yaml",
        "csrf/samesite/front-envoy.yaml",
        "csrf/service-envoy.yaml",
        "ext_authz/config/grpc-service/v2.yaml",
        "ext_authz/config/grpc-service/v3.yaml",
        "ext_authz/config/http-service.yaml",
        "ext_authz/config/opa-service/v2.yaml",
        "fault-injection/envoy.yaml",
        "front-proxy/front-envoy.yaml",
        "front-proxy/service-envoy.yaml",
        "grpc-bridge/client/envoy-proxy.yaml",
        "grpc-bridge/server/envoy-proxy.yaml",
        "jaeger-tracing/front-envoy-jaeger.yaml",
        "jaeger-tracing/service1-envoy-jaeger.yaml",
        "jaeger-tracing/service2-envoy-jaeger.yaml",
        "load-reporting-service/service-envoy-w-lrs.yaml",
        "lua/envoy.yaml",
        "lua/lib/mylibrary.lua",
        "mysql/envoy.yaml",
        "redis/envoy.yaml",
        "zipkin-tracing/front-envoy-zipkin.yaml",
        "zipkin-tracing/service1-envoy-zipkin.yaml",
        "zipkin-tracing/service2-envoy-zipkin.yaml",
    ],
)
