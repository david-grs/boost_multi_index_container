# boost::multi\_index\_container...
... is awesome.

snippets, tests, benchmarks on boost::multi\_index\_container



```
boost::mic 1 index <insert 1000000 elements>: total_time=1903ms per_iteration=1903.4ns
boost::mic 1 index <lookup 100 elements>: total_time=181us per_iteration=1819.89ns
boost::mic 1 index <insert 100 elements>: total_time=252us per_iteration=2526.22ns
boost::mic 1 index <container walk>: total_time=162ms per_iteration=162.864ns
malloc_calls=2000208 bytes_allocated=1014M

boost::mic 2 indexes <insert 1000000 elements>: total_time=4026ms per_iteration=4026.79ns
boost::mic 2 indexes <lookup 100 elements>: total_time=179us per_iteration=1794.1ns
boost::mic 2 indexes <insert 100 elements>: total_time=521us per_iteration=5214.62ns
boost::mic 2 indexes <container walk>: total_time=168ms per_iteration=168.964ns
malloc_calls=2000208 bytes_allocated=1037M

boost::mic 4 indexes <insert 1000000 elements>: total_time=5621ms per_iteration=5621.85ns
boost::mic 4 indexes <lookup 100 elements>: total_time=161us per_iteration=1611.97ns
boost::mic 4 indexes <insert 100 elements>: total_time=706us per_iteration=7068.6ns
boost::mic 4 indexes <container walk>: total_time=157ms per_iteration=157.05ns
malloc_calls=2000208 bytes_allocated=1083M

boost::mic 8 indexes <insert 1000000 elements>: total_time=8990ms per_iteration=8990ns
boost::mic 8 indexes <lookup 100 elements>: total_time=181us per_iteration=1817.29ns
boost::mic 8 indexes <insert 100 elements>: total_time=1ms per_iteration=12040.4ns
boost::mic 8 indexes <container walk>: total_time=160ms per_iteration=160.631ns
malloc_calls=2000208 bytes_allocated=1175M

boost::mic 16 indexes <insert 1000000 elements>: total_time=16181ms per_iteration=16181.2ns
boost::mic 16 indexes <lookup 100 elements>: total_time=193us per_iteration=1934.62ns
boost::mic 16 indexes <insert 100 elements>: total_time=1ms per_iteration=19120.3ns
boost::mic 16 indexes <container walk>: total_time=166ms per_iteration=166.237ns
malloc_calls=2000208 bytes_allocated=1358M

std::vector<A> <insert 1000000 elements>: total_time=553ms per_iteration=553.505ns
std::vector <sort>: 129ms
std::vector<A> <lookup 100 elements>: total_time=63us per_iteration=635.92ns
std::vector<A> <insert 100 elements>: total_time=68us per_iteration=682.32ns
std::vector<A> <container walk>: total_time=4ms per_iteration=4.16314ns
malloc_calls=1000128 bytes_allocated=1008M

std::vector<B> <insert 1000000 elements>: total_time=694ms per_iteration=694.199ns
std::vector <sort>: 657ms
std::vector<B> <lookup 100 elements>: total_time=193us per_iteration=1939.3ns
std::vector<B> <insert 100 elements>: total_time=88us per_iteration=884.76ns
std::vector<B> <container walk>: total_time=38ms per_iteration=38.9865ns
malloc_calls=3000328 bytes_allocated=1032M

std::multiset <insert 1000000 elements>: total_time=1811ms per_iteration=1811.62ns
std::multiset <lookup 100 elements>: total_time=141us per_iteration=1410.05ns
std::multiset <insert 100 elements>: total_time=213us per_iteration=2130.93ns
std::multiset <container walk>: total_time=148ms per_iteration=148.317ns
malloc_calls=2000207 bytes_allocated=1022M

boost.flat_set <insert 1000000 elements>: total_time=234843ms per_iteration=234843ns
boost.flat_set <lookup 100 elements>: total_time=94us per_iteration=949.07ns
boost.flat_set <insert 100 elements>: total_time=22ms per_iteration=225400ns
boost.flat_set <container walk>: total_time=2ms per_iteration=4.27402ns
malloc_calls=1000128 bytes_allocated=1008M
```
