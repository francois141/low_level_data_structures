# Concurrent low level data structures

## Priority queue

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|       52,760,735.67 |               18.95 |    2.5% |      3.13 | `basic_prio_queue`
|       57,954,987.83 |               17.25 |    4.1% |      3.46 | `basic_prio_queue_lockfree`
