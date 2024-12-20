import math
import sys


class Reader:
    def __init__(self, file_path):
        self.file_path = file_path
        self.file = None
        self.buffer = []

    def open(self):
        try:
            self.file = open(self.file_path, 'r')
        except FileNotFoundError:
            print(f"[Ali] The file '{self.file_path}' does not exist.")
        except Exception as e:
            print(f"[Ali] {e}")

    def close(self):
        if self.file:
            self.file.close()

    def read_window(self, window_size, mode="split"):
        if not self.file:
            print("[Ali] File is not open.")
            return

        if mode == "split":
            window = []
            for line in self.file:
                window.append(line.strip())
                if len(window) == window_size:
                    yield window
                    window = []
            if window:
                yield window

        elif mode == "sliding":
            for line in self.file:
                self.buffer.append(line.strip())
                if len(self.buffer) >= window_size:
                    yield self.buffer[:window_size]
                    self.buffer = self.buffer[1:]
        else:
            print(f"[Ali] Unsupported mode '{mode}'. Use 'split' or 'sliding'.")


class Profile:
    def __init__(self):
        self.profile = {}

    def add_profile(self, bin, count):
        if bin in self.profile:
            self.profile[bin] += count
        else:
            self.profile[bin] = count


class StrideProfile(Profile):

    def get_spatial_locality(self):
        spatial_locality = 0
        total_references = sum(self.profile.values())
        for bin, value in self.profile.items():
            spatial_locality += (value / total_references) / bin
        return spatial_locality


class ReuseProfile(Profile):

    def set_total_memory_accesses(self, total_memory_accesses):
        self.total_memory_accesses = total_memory_accesses

    def get_temporal_locality(self):
        temporal_locality = 0
        for bin, value in self.profile.items():
            temporal_locality += (math.pow(2, bin) * value) / self.total_memory_accesses
        return temporal_locality


trace_path = "traces/test.trc"
window_size = 32
window_mode = "split"
word_size = 4

stride_profile = StrideProfile()
reuse_profile = ReuseProfile()

total_memory_accesses = 0
total_compute_inst = 0

reader = Reader(trace_path)
reader.open()

for window in reader.read_window(window_size, window_mode):
    addresses = []
    visited = [0] * window_size
    min_stride = sys.maxsize

    if window_mode == "split":
        total_memory_accesses += window_size

    elif window_mode == "sliding":
        total_memory_accesses += 1
        total_compute_inst += int(window[0].split()[0])

    for request in window:
        addresses.append(int(request.split()[1], 16))
        if window_mode == "split":
            total_compute_inst += int(request.split()[0])

    for i in range(len(addresses)):
        reuse_counter = 0
        for j in range(i + 1, len(addresses)):
            current_stride = math.floor(abs(addresses[i] - addresses[j]) / word_size)
            if current_stride != 0 and current_stride < min_stride:
                min_stride = current_stride
            if current_stride == 0 and not visited[j]:
                visited[j] = 1
                reuse_counter += 1
        if reuse_counter != 0:
            reuse_profile.add_profile(math.floor(math.log2(reuse_counter)), 1)
    stride_profile.add_profile(min_stride, 1)

reader.close()

arithmetic_intensity = total_compute_inst / total_memory_accesses

spatial_locality = stride_profile.get_spatial_locality()

reuse_profile.set_total_memory_accesses(total_memory_accesses)
temporal_locality = reuse_profile.get_temporal_locality()

print("trace_path = ", f"{trace_path}")
print("total_compute_inst = ", f"{total_compute_inst}")
print("total_memory_accesses = ", f"{total_memory_accesses}")
print("arithmetic_intensity = ", f"{arithmetic_intensity:.2f}")
print("spatial_locality = ", f"{spatial_locality:.2f}")
print("temporal_locality = ", f"{temporal_locality:.2f}")
