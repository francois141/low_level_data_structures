# Concurrent low level data structures

## Priority queue - lock free implementation has race conditions

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|        6,380,464.50 |              156.73 |    0.8% |      1.18 | `basic_prio_queue`
|        4,252,044.20 |              235.18 |    0.6% |      0.75 | `basic_prio_queue_lockfree`