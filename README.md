# Concurrent low level data structures

### Benchmark minimum simd

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|          883,180.35 |            1,132.27 |    0.2% |      0.16 | `naive_argmin`
|          389,363.07 |            2,568.30 |    0.7% |      0.07 | `simd_naive_argmin`

### Priority queue benchmark 

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|        6,550,541.31 |              152.66 |    2.9% |      1.19 | `basic_prio_queue`
|        4,344,657.25 |              230.17 |    0.4% |      0.77 | `basic_prio_queue_lockfree`

### Benchmark stack

|               ns/op |                op/s |    err% |     total | benchmark
|--------------------:|--------------------:|--------:|----------:|:----------
|           37,243.44 |           26,850.36 |    2.7% |      0.01 | `naive_stack`
|           36,579.27 |           27,337.89 |    2.1% |      0.01 | `lock_free_stack`
