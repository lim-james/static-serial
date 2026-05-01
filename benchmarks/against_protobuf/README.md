# STSE vs G Protobuf

Compiled both on `-O2`

Here are the numbers on a data set with 1M entries.


| | STSE | Protobuf | Magnitude |
| --- | --- | --- | --- |
| Serialization | 177ms | 594ms | 3.5x |
| Deserialization | 60 | 1354ms | 23x  |
| Binary Size | 321 MB | 489MB | 1.52x smaller |


## Trade-offs

* My lib only supports fixed schema.
* No forward/backward compatibility.
* No support for schema evolution.
* No dynamic sized containers.

