# ping data
data = '\x75\x65\x01\x04\x04\xF1\x01\x00'
# cs: D5, 6A -> OK

# example data
data = '\x75\x65\x80\x0E\x0E\x04\x3E\x7A\x63\xA0\xBB\x8E\x3B\x29\x7F\xE5\xBF\x7F'
# cs: 84 EE -> OK

# idle mode
data = '\x75\x65\x01\x02\x02\x02'
# cs: E1 C7 -> OK

# config imu datastream format
data = '\x75\x65\x0C\x0D\x0D\x08\x01\x03\x12\x00\x0A\x04\x00\x0A\x05\x00\x0A'
# cs: 45 F2 -> OK

# Configure the Estimation Filter Data-stream Format
data = '\x75\x65\x0C\x10\x10\x0A\x01\x04\x11\x00\x0A\x05\x00\x0A\x0D\x00\x0A\x0E\x00\x0A'
# cs: 6E B0 -> OK

# Save the IMU and Estimation Filter MIP Message Format
data = '\x75\x65\x0C\x08\x04\x08\x03\x00\x04\x0A\x03\x00'
# cs: 0E31 -> OK

# Enable the IMU and Estimation Filter Data-streams
data = '\x75\x65\x0C\x0A\x05\x11\x01\x01\x01\x05\x11\x01\x03\x01'
# cs: 24 CC -> OK

# Resume the Device
data = '\x75\x65\x01\x02\x02\x06'
# cs: E5 CB -> OK

# Initialize the Filter
data = '\x75\x65\x0C\x07\x07\x01\x00\x01\x0C\x00\x00'
# cs: 02 FC -> OK

# Initialize attitude
data = '\x75\x65\x0D\x0E\x0E\x02\xBA\xE3\xED\x9B\x3C\x7D\x6D\xDF\xBF\x85\x5C\xF5'
# cs: C4 09 -> OK

# Poll IMU
data = '\x75\x65\x0C\x04\x04\x01\x00\x00'
# cs: EFDA -> OK

# IMU Response Field 1
data = '\x75\x65\x80\x1C\x0E\x04\x3E\x7A\x63\xA0\xBB\x8E\x3B\x29\x7F\xE5\xBF\x7F'
# cs: 0x41 0xBB -> NOT OK

# IMU Response Field 2
data = '\x0E\x03\x3E\x7A\x63\xA0\xBB\x8E\x3B\x29\x7F\xE5\xBF\x7F'
# cs: 0xAD 0xDC -> NOT OK

# IMU Response Field 1&2
data = '\x75\x65\x80\x1C\x0E\x04\x3E\x7A\x63\xA0\xBB\x8E\x3B\x29\x7F\xE5\xBF\x7F\x0E\x03\x3E\x7A\x63\xA0\xBB\x8E\x3B\x29\x7F\xE5\xBF\x7F'
# cs: 0xAD 0xDC -> OK

# Random IMU response processed
data = '\x75\x65\x80\x2a\x0e\x04\x3c\xbd\x03\x89\x3e\x02\x7a\x13\x3f\x7d\xc1\x99\x0e\x05\xba\x04\xa7\xd1\xb9\xa2\x0c\x42\xb7\xc8\x95\x40\x0e\x12\x40\xa5\x6b\x02\x0c\x49\xba\x5e\x00\x06\x29\x68'
# cs:  ba 2e  -- program calculated ok

# IMU response
data = '\x75\x65\x80\x2a\x0e\x04\x3c\xbd\x03\x89\x3e\x02\x7a\x13\x3f\x7d\xc1\x99\x0e\x05\xba\x04\xa7\xd1\xb9\xa2\x0c\x42\xb7\xc8\x95\x40\x0e\x12\x40\xa5\x6b\x02\x0c\x49\xba\x5e\x00\x06\x29\x68'
#data = '\x75\x65\x80\x2a\x0\xe\x4\x3c\xbd\x3\x89\x3e\x2\x7a\x13\x3f\x7d\xc1\x99\xe\x5\xba\x4\xa7\xd1\xb9\xa2\xc\x42\xb7\xc8\x95\x40\xe\x12\x40\xa5\x6b\x2\xc\x49\xba\x5e\x0\x6\x29\x68'
# cs:  75 65 - NOT OK

# random for 9600 baud
data = 'x75\x65\x0C\x07\x07\x40\x01\x00\x03\x84\x00'
# 75 65 0C 07 07 40              01  00  01  C2  00
# cs: 0x49, 0x9

checksum1 = 0
checksum2 = 0
for ch in data:
    checksum1 += ord(ch)  # ord('\x03`) -> 3
    checksum2 += checksum1

checksum1 = sum(map(ord, data))

while(checksum1>256):
    checksum1 = checksum1 - 256
while(checksum2>256):
    checksum2 = checksum2 - 256

checksum1 = hex(checksum1)
checksum2 = hex(checksum2)
print(checksum1, checksum2)