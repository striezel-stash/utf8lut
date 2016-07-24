#include "Buffer/BaseBufferProcessor.h"
#include "Buffer/ProcessorPlugins.h"
#include "Base/Timing.h"
#include "Buffer/BufferDecoder.h"
#include <stdio.h>

void ProcessFiles(BaseBufferProcessor &processor, FILE *fi, FILE *fo) {
	assert(fi && fo);
	processor.Clear();
	InteractiveInput input(processor);
	InteractiveOutput output(processor);

	while (!feof(fi)) {
		//ask where to write input data
		char *inputBuffer;
		int maxSize;
		input.GetBuffer(inputBuffer, maxSize);
		//read bytes from input
		int readSize = fread(inputBuffer, 1, maxSize, fi);
		//tell how many bytes we really have
		input.ConfirmInputBytes(readSize, !!feof(fi));

		//do all the work
		bool ok = processor.Process();

		//check if hard error occurred
		if (!ok)
			throw "Input data is invalid!";
		//ask how many output buffers are there
		int streamsCnt = processor.GetStreamsCount();
		for (int k = 0; k < streamsCnt; k++) {
			//get output bytes
			const char *outputBuffer;
			int outSize;
			output.GetBuffer(outputBuffer, outSize, k);
			//write them to file
			fwrite(outputBuffer, 1, outSize, fo);
		}
	}

	//check if some bytes in the input remain
	if (input.GetRemainingDataSize() != 0)
		throw "Input data is incomplete!";
}

void ProcessInMemory(BaseBufferProcessor &processor, const char *inputBuffer, int inputSize, char *outputBuffer, int &outputSize) {
	if (!outputBuffer) {
		outputSize = processor.GetOutputBufferMinSize(inputSize);
		return;
	}
	assert(inputBuffer);
	processor.Clear();
	ContiguousInput input(processor, inputBuffer, inputSize);
	ContiguousOutput output(processor, outputBuffer, outputSize);

	while (!input.Finished()) {
		//do all the work
		bool ok = processor.Process();

		//check if hard error occurred
		if (!ok)
			throw "Input data is invalid!";
	}

	//check if some bytes in the input remain
	if (input.GetRemainingDataSize() != 0)
		throw "Input data is incomplete!";
	
	outputSize = output.GetTotalOutputSize();
}

void ProcessFilesByName(BaseBufferProcessor &processor, const char *nameI, const char *nameO) {
	FILE *fi = fopen(nameI, "rb");
	FILE *fo = fopen(nameO, "wb");
	ProcessFiles(processor, fi, fo);
	fclose(fi);
	fclose(fo);
}

int main() {
	
try {
	//decode file (multiple times for profiling)
	for (int run = 0; run < 100; run++) {
		BufferDecoder<3, 2, dmValidate, 4> decoder;
		ProcessFilesByName(decoder, "utf8.txt", "utfXX.txt");
	}
/*
	//encode file (multiple times for profiling)
	for (int run = 0; run < 100; run++)
		ProcessFilesByName(encoder, "utfXX.txt", "utf8.txt");*/

	//print profiling info
	TIMING_PRINT();
} catch(const char *str) {
	fprintf(stderr, "Error: %s\n", str);
}

	return 0;
}