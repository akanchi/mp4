# MP4 Demuxer

我的目的是将`mp4`文件中的`aac`和`avc`解析出来，并保存为对应媒体文件。

众所周知，`mp4`的媒体数据是保存在`mdat`中，但 `mdat`没有告诉我们哪些是音频/视频数据，所以需要依赖其它`box`（主要是`stco`、`stsz`、`stsc`）去定位`mdat`中的音视频数据。

#### stco(Chunk Offset Box)

用于定位媒体数据中的块偏移，因为有多个块，所以是一个数组格式，chunk_offsets[number_of_entries]

#### stsz(Sample Size Box)

用于表示每个媒体`sample`的的大小，通常`sample`的大小是可变的，sample_sizes[number_of_entries]保存所有sample的size

#### stsc(Sample-to-Chunk Box)

如果只知道`stco` 和`stsz`是无法定位媒体数据的，所以需要`sample`和`chunk`的对应表，才能解析出媒体数据

以下表格来自于`docs/qtff.pdt`

**Figure 2-47**    An example of a sample-to-chunk table

| First chunk | Samples per chunk | Sample description ID |
| :---------: | :---------------: | :-------------------: |
|      1      |         3         |           1           |
|      3      |         1         |           1           |
|      5      |         1         |           1           |

下面是展开之后的表格，可以看出可以通过`stco`去索引`chunk`，以及通过`stsz`去索引`sample`。

|       |     First chunk      |   Samples per chunk   | Sample description ID |
| :---: | :------------------: | :-------------------: | :-------------------: |
|       |          1           |           3           |           1           |
| ***** |          2           |           3           |           1           |
|       |          3           |           1           |           1           |
| ***** |          4           |           1           |           1           |
|       |          5           |           1           |           1           |
|       | Number of chunks = 5 | Number of samples = 9 |                       |

#### 提取媒体数据

知道了所需要的信息就可以着手提取媒体数据了，相关代码可以参考`int TrackContext::extract()`

```c++
uint32_t current_entry_index = 0;// stsc中，entries的下标
uint32_t current_entry_sample_index = 0; // stsc中，对应current_entry_index的entry.sample的下标
uint32_t chunk_logic_index = 0;	// stco中，chunk_offsets的下标
uint32_t sample_offset = 0;// sample的偏移量
for (int i = 0; i < sample_sizes.count; i++) {
  // 1. 取出当前entry = stsc->entries[current_entry_index]
  // 2. 计算当前sample的起始位置pos = chunk_offsets[chunk_logic_index] + sample_offset
  // 3. 起始位置pos 和 sample_sizes[i]可以获取当前的sample数据
  // 4. current_entry_sample_index++
  // 5. 计算sample_offset += sample_sizes[i]
  // 6. 如果current_entry_sample_index >= entry.samples_per_chunk 则切换到下一个chunk，并重置sample相关标记
}
```

提取avc的时候，一个sample可能含有多个nalu，格式是``` [nalu length(4bytes)][nalu data(nalu length bytes)]+[nalu length][nalu data]+...```

