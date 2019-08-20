/* The Akarin CPU Simulator */
/* written by Geng-You Chen */
/* gengyouchen[at]gmail.com */

#ifndef __AKARIN_H__
#define __AKARIN_H__

#include <stdint.h>
#include <string>
#include <DRAMSim.h>

class Akarin
{
	private:
		DRAMSim::MultiChannelMemorySystem *memorySource;

		typedef	DRAMSim::Callback<
			Akarin,
			void,
			unsigned,
			uint64_t,
			uint64_t> MemoryCallback;

		MemoryCallback readFinishedCallback;
		MemoryCallback writeFinishedCallback;

		uint64_t currentCPUCycle;
		uint64_t stalledReadAddress;
		uint64_t stalledWriteAddress;

		void onReadFinished(
				unsigned channelID,
				uint64_t logicalAddress,
				uint64_t currentDRAMCycle);

		void onWriteFinished(
				unsigned channelID,
				uint64_t logicalAddress,
				uint64_t currentDRAMCycle);

		void printAccessMessage(
				bool isFinished,
				bool isWrite,
				uint64_t logicalAddress) const;

		uint64_t addressAligned(
				uint64_t logicalAddress) const;

		void addressMapping(
				uint64_t logicalAddress,
				unsigned &channelID,
				unsigned &rankID,
				unsigned &bankID,
				unsigned &rowID,
				unsigned &columnID) const;

		void reverseAddressMapping(
				uint64_t &logicalAddress,
				unsigned channelID,
				unsigned rankID,
				unsigned bankID,
				unsigned rowID,
				unsigned columnID) const;

	public:
		Akarin(
				const std::string &dev,
				const std::string &sys,
				const std::string &pwd,
				const std::string &trc,
				unsigned megsOfMemory,
				uint64_t cpuClkFreqHz);

		~Akarin();

		void accessMemory(
				unsigned channelID,
				unsigned rankID,
				unsigned bankID,
				unsigned rowID,
				unsigned columnID,
				bool isWrite,
				bool isStalled);

		void accessMemory(
				uint64_t logicalAddress,
				bool isWrite,
				bool isStalled);

		void doComputing(
				uint64_t busyCPUCycle);
};

#endif
