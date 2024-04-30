# webcachesim:

We replay production traffic from a CDN server operated by the [Wikimedia Foundation](https://wikimediafoundation.org/wiki/Home), and consider various modern caching policies.

<img src="https://cloud.githubusercontent.com/assets/9959772/22966302/de4de664-f361-11e6-9345-854bffa2005c.png" width=500px />

## Compiling webcachesim

You will need a compiler that supports C++11, e.g., GCC 4.8.1 upwards (with -std=c++11). Older compilers that partially support C++11, e.g., GCC 4.4, can compile (with -std=c++0x).
Metaflow and other libraries

Then, get started with

    make
    pip install -r requirements.txt

To Run the metaflow

    python3 train_trace_classifier.py run


## Using an exisiting policy

The basic interface is

    ./webcachesim traceFile cacheType cacheSize [cacheParams]

where

 - traceFile: a request trace (see below)
 - cacheType: one of the caching policies (see below)
 - cacheSize: the cache capacity in bytes
 - cacheParams: optional cache parameters, can be used to tune cache policies (see below)

### Request trace format

Request traces must be given in a space-separated format with three colums
- time should be a long long int, but can be arbitrary (for future TTL feature, not currently in use)
- id should be a long long int, used to uniquely identify objects
- size should be a long long int, this is object's size in bytes

| time |  id | size |
| ---- | --- | ---- |
|   1  |  1  |  120 |
|   2  |  2  |   64 |
|   3  |  1  |  120 |
|   4  |  3  |  14  |
|   4  |  1 |  120 |

Example trace in file "test.tr".

### Available caching policies

There are currently ten caching policies. This section describes each one, in turn, its parameters, and how to run it on the "test.tr" example trace with cache size 1000 Bytes.

#### LRU

does: least-recently used eviction

params: none

example usage:

    ./webcachesim test.tr LRU 1000
     
#### FIFO

does: first-in first-out eviction

params: none

example usage:

    ./webcachesim test.tr FIFO 1000
    
#### GDS

does: greedy dual size eviction

params: none

example usage:

    ./webcachesim test.tr GDS 1000
    
#### GDSF

does: greedy dual-size frequency eviction

params: none

example usage:

    ./webcachesim test.tr GDSF 1000
    
#### LFU-DA

does: least-frequently used eviction with dynamic aging

params: none

example usage:

    ./webcachesim test.tr LFUDA 1000
    
    
#### Filter-LRU

does: LRU eviction + admit only after N requests

params: n - admit after n requests)

example usage (admit after 10 requests):

    ./webcachesim test.tr Filter 1000 n=10
    
#### Threshold-LRU

does: LRU eviction + admit only after N requests

params: t - the size threshold in log form (base 2)

example usage (admit only objects smaller than 512KB):

    ./webcachesim test.tr ThLRU 1000 t=19
    
#### ExpProb-LRU

does: LRU eviction + admit with probability exponentially decreasing with object size

params: c - the size which has a 50% chance of being admitted (used to determine the exponential family)

example usage (admit objects with size 256KB with about 50% probability):

    ./webcachesim test.tr ExpLRU 1000 c=18
  
#### LRU-K

does: evict object which has oldest K-th reference in the past

params: k - eviction based on k-th reference in the past

example usage (each segment gets half the capacity)

    ./webcachesim test.tr LRUK 1000 k=4
