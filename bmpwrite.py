# -*- coding: utf-8 -*-
import sys, struct

filename = sys.argv[1] if len(sys.argv)>1 else "input.txt"
lines = open(filename).readlines()
N, M, color = map(int, lines[0].split())

# rows - список строк *в порядке снизу вверх*, каждая строка - список целых чисел. 
# Если изображение полноцветное ширины, например, 100, то каждая строка содержит 300 чисел,
# из которых первые три - это компоненты R, G, B пикселя в верхнем левом углу
rows = [list(map(int, line.split())) for line in lines[-1:0:-1]]

BMFheader = BMIheader = palette = b''

bitperpixel = 24 if color else 8
# BMFheader = ...
BMIheader = struct.pack('<iiihhiiiiii', 40, N, M, 1, bitperpixel, 0, 0, 0, 0, 0, 0)

if color:
	pixeldata = b'' # ...
else:
	palette = b''.join([struct.pack('BBBB', i,i,i,0) for i in range(256)])
	pixeldata = b'' # ...

open(filename+".bmp","wb").write(BMFheader + BMIheader + palette + pixeldata)
