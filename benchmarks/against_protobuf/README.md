# STSE vs G Protobuf

Comiled both on `-O2`

Here are the numbers on a data set with 1M entries.

| | STSE | Protobuf | Magnitude |
| --- | --- | --- | --- |
| Serialization | 244ms | 2858ms | 12.1x |
| Deserialization | 78ms | 3637ms | 47.9x  |
| Binary Size | 321 MB | 489MB | 1.52x smaller |


## Trade-offs

My lib only supports fixed schema. No forward/backward compatibility - no
support for schema evolution.
