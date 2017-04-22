#!/bin/sh

#
# Call server at specified port
#
port=8222

curl -X POST -H "Content-Type: application/json" \
    -d '{"id": 1, "method": "Test.Greet", "params": [{"name":"world"}]}' \
    http://localhost:$port/test

# Expected reply:
# {"id":1,"result":"Hello world","error":null}
