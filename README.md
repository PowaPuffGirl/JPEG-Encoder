# JPEG Encoder

This is a PPM to JPG/JPEG encoder using SIMD. It was developed for a lecture at
FHWS and is primarily designed for speed. Because of this it only encodes to
three channels (no black/white) and only with 4:2:0 subsampling, but it should
be rather easy to fit it to other purposes.

For now it can encode a 4K image in under a second and a 12K (12000x6660) image
in about 3 seconds.


## Architecture

This project has 3 targets: The encoder itself (`MedienInfo`, named after
the lecture), benchmarks using Google Benchmark (`Benchmarks`) and a helper
for creating test PPM images (`ppmCreator`).

### Medieninfo

The entrypoint is in `main.cpp`. From there, an instance of `PPMParser` will be
created. That class reads the PPM headers and creates a storage buffer
(`BlockwiseRawImage`) which stores the data in blocks. Those blocks are already
being averaged (for subsampling) and converted to YCbCr when writing and are
created by a separate thread started by the `PPMParser`-instance.

After that an instance of `ImageProcessor` is created (contained in
`EncodingProcessor.h`) to actually process the image. This class is templated
with the transformation algorithm to use (those are contained in the `dct/`
folder, three are available). It handles processing the blocks with the
transform after the blocks are read by the other thread, writing the image
metadata and writing out the blocks in order. The quantisation and encoding is
done by the `OffsetSampledWriter` class (in `SampledWriter.h`). This class also
handles the data collection for the Huffman trees, which are created by the
`ImageProcessor` instance and used by the `HuffmanEncoder` class, which handles
the final conversion of an encoded block to a bit stream. Lastly, the bit stream
is written out to the file in the main.

Usage of the binary: `./MedienInfo image.ppm`. This will create a jpeg file
with the same name next to it. To benchmark the conversion you can put a
minimal runtime in seconds as second parameters and it will rerun the
conversion until the runtime is reached and then output the average runtime.
Example: `./MedienInfo image.ppm 10` (this will run at least 10s).

### Benchmarks

This executable uses Google Benchmarks to run several benchmarks. All
benchmarks are in the `benchmarks` subfolder, but most are disabled because
for the final mark only the speed of the transformation and the encoding itself
was relevant.

### PPMcreator

Simple edit the main method in `ppmCreatorMain.cpp` to create test images
for debugging and showcasing the encoder. The first parameter to `writePPM` is
the image name. When running an image with the name `${imagename}.ppm` will
be created in the `output/` folder (that should exist).

## License

This project uses the MIT license. A copy of the license is provided in the
`LICENSE` file.

To make builds on Windows possible the headers and a binary
build for Vc are contained in the repository. These parts use a BSD 3 license,
which is contained in the `Vc/` subfolder.
