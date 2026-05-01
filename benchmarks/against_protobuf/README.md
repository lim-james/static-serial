# STSE vs G Protobuf

Compiled both on `-O2`

Here are the numbers on a data set with 1M entries.

| | STSE | Protobuf | Magnitude |
| --- | --- | --- | --- |
| Serialization | 171ms | 2941ms | 17x |
| Deserialization | 60 | 3637ms | 60x  |
| Binary Size | 321 MB | 489MB | 1.52x smaller |


## Trade-offs

My lib only supports fixed schema. No forward/backward compatibility - no
support for schema evolution.
