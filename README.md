# Spiral Knights

Inspired by a video from [Numberphile](https://www.youtube.com/watch?v=UiX4CFIiegM)
on knights attacking each other on a spiral.

The C++ program creates a PNG of the resulting pattern. The program takes 3 optional arguments:

- The number of knights (defaults to 2)
- The side length of the spiral (defaults to 1000)
- The name of the PNG file (defaults to knights.png)

The program is quite fast. On my lowly Intel i3770 from 2008, generating a spiral
of side 1000 is nearly instantaneous. One of size 10 000 takes a few seconds.

Here is an image showing the result of 4 knights in a 10 000 x 10 000 spiral:

![4 knights 10k spiral](4-colors-10k-spiral-knights.png)
