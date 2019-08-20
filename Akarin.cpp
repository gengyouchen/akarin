/* The Akarin CPU Simulator */
/* written by Geng-You Chen */
/* gengyouchen[at]gmail.com */

#include <assert.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <DRAMSim.h>
#include <AddressMapping.h>
#include <SystemConfiguration.h>
#include "Akarin.h"

Akarin::Akarin(
		const std::string &dev,
		const std::string &sys,
		const std::string &pwd,
		const std::string &trc,
		unsigned megsOfMemory,
		uint64_t cpuClkFreqHz) :
	readFinishedCallback(this, &Akarin::onReadFinished),
	writeFinishedCallback(this, &Akarin::onWriteFinished),
	currentCPUCycle(0),
	stalledReadAddress(-1),
	stalledWriteAddress(-1)
{
	memorySource = DRAMSim::getMemorySystemInstance(
			dev,
			sys,
			pwd,
			trc,
			megsOfMemory);

	memorySource->setCPUClockSpeed(cpuClkFreqHz);

	memorySource->RegisterCallbacks(
			&readFinishedCallback,
			&writeFinishedCallback,
			NULL);
}

Akarin::~Akarin()
{
	delete memorySource;
}

void Akarin::doComputing(
		uint64_t busyCPUCycle)
{
	for (int i = 0; i < busyCPUCycle; i++) {
		currentCPUCycle++;
		memorySource->update();
	}
}

void Akarin::accessMemory(
		unsigned channelID,
		unsigned rankID,
		unsigned bankID,
		unsigned rowID,
		unsigned columnID,
		bool isWrite,
		bool isStalled)
{
	uint64_t logicalAddress;
	reverseAddressMapping(
			logicalAddress,
			channelID,
			rankID,
			bankID,
			rowID,
			columnID);

	accessMemory(logicalAddress, isWrite, isStalled);
}

void Akarin::accessMemory(
		uint64_t logicalAddress,
		bool isWrite,
		bool isStalled)
{
	logicalAddress = addressAligned(logicalAddress);
	memorySource->addTransaction(isWrite, logicalAddress);

	printAccessMessage(false, isWrite, logicalAddress);

	if (isStalled) {

		if (isWrite) {

			stalledWriteAddress = logicalAddress;
			do {
				currentCPUCycle++;
				memorySource->update();
			} while (stalledWriteAddress != -1);

		} else {

			stalledReadAddress = logicalAddress;
			do {
				currentCPUCycle++;
				memorySource->update();
			} while (stalledReadAddress != -1);

		}
	}
}

void Akarin::onReadFinished(
		unsigned channelID,
		uint64_t logicalAddress,
		uint64_t currentDRAMCycle)
{
	if (stalledReadAddress == logicalAddress) {
		stalledReadAddress = -1;
	}

	printAccessMessage(true, false, logicalAddress);
}

void Akarin::onWriteFinished(
		unsigned channelID,
		uint64_t logicalAddress,
		uint64_t currentDRAMCycle)
{
	if (stalledWriteAddress == logicalAddress) {
		stalledWriteAddress = -1;
	}

	printAccessMessage(true, true, logicalAddress);
}

void Akarin::printAccessMessage(
		bool isFinished,
		bool isWrite,
		uint64_t logicalAddress) const
{
	unsigned channelID;
	unsigned rankID;
	unsigned bankID;
	unsigned rowID;
	unsigned columnID;

	addressMapping(
			logicalAddress,
			channelID,
			rankID,
			bankID,
			rowID,
			columnID);

	if (isFinished) {
		std::cout << "Finish ";
	} else {
		std::cout << "Start ";
	}

	if (isWrite) {
		std::cout << "writing ";
	} else {
		std::cout << "reading ";
	}

	std::cout << std::hex;
	std::cout << "0x" << logicalAddress << " ";

	std::cout << std::dec;
	std::cout << "(ch=" << channelID << ", ";
	std::cout << "rank=" << rankID << ", ";
	std::cout << "bank=" << bankID << ", ";
	std::cout << "row=" << rowID << ", ";
	std::cout << "col=" << columnID << ") ";

	std::cout << "at CPU cycle " << currentCPUCycle;
	std::cout << std::endl;
}

uint64_t Akarin::addressAligned(
		uint64_t logicalAddress) const
{
	unsigned transactionSize = (JEDEC_DATA_BUS_BITS / 8) * BL;
	uint64_t transactionMask = transactionSize - 1;
	return logicalAddress & ~transactionMask;
}

void Akarin::addressMapping(
		uint64_t logicalAddress,
		unsigned &channelID,
		unsigned &rankID,
		unsigned &bankID,
		unsigned &rowID,
		unsigned &columnID) const
{

#ifndef NDEBUG

	unsigned transactionSize = (JEDEC_DATA_BUS_BITS / 8) * BL;
	uint64_t transactionMask =  transactionSize - 1;
	unsigned channelBitWidth = DRAMSim::dramsim_log2(NUM_CHANS);
	unsigned rankBitWidth = DRAMSim::dramsim_log2(NUM_RANKS);
	unsigned bankBitWidth = DRAMSim::dramsim_log2(NUM_BANKS);
	unsigned rowBitWidth = DRAMSim::dramsim_log2(NUM_ROWS);
	unsigned colBitWidth = DRAMSim::dramsim_log2(NUM_COLS);
	unsigned byteOffsetWidth = DRAMSim::dramsim_log2((JEDEC_DATA_BUS_BITS / 8));
	unsigned colLowBitWidth = DRAMSim::dramsim_log2(transactionSize) - byteOffsetWidth;
	unsigned colHighBitWidth = colBitWidth - colLowBitWidth;

	assert((logicalAddress & transactionMask) == 0);

	uint64_t remainedAddress = logicalAddress;
	remainedAddress >>= byteOffsetWidth;
	remainedAddress >>= colLowBitWidth;
	remainedAddress >>= rowBitWidth;
	remainedAddress >>= colHighBitWidth;
	remainedAddress >>= bankBitWidth;
	remainedAddress >>= rankBitWidth;
	remainedAddress >>= channelBitWidth;
	assert(remainedAddress == 0);

#endif

	DRAMSim::addressMapping(
			logicalAddress,
			channelID,
			rankID,
			bankID,
			rowID,
			columnID);

}

void Akarin::reverseAddressMapping(
		uint64_t &logicalAddress,
		unsigned channelID,
		unsigned rankID,
		unsigned bankID,
		unsigned rowID,
		unsigned columnID) const
{

#ifndef NDEBUG

	assert(channelID < NUM_CHANS);
	assert(rankID < NUM_RANKS);
	assert(bankID < NUM_BANKS);
	assert(rowID < NUM_ROWS);
	assert(columnID < NUM_COLS);

#endif

	unsigned transactionSize = (JEDEC_DATA_BUS_BITS / 8) * BL;
	unsigned channelBitWidth = DRAMSim::dramsim_log2(NUM_CHANS);
	unsigned rankBitWidth = DRAMSim::dramsim_log2(NUM_RANKS);
	unsigned bankBitWidth = DRAMSim::dramsim_log2(NUM_BANKS);
	unsigned rowBitWidth = DRAMSim::dramsim_log2(NUM_ROWS);
	unsigned colBitWidth = DRAMSim::dramsim_log2(NUM_COLS);
	unsigned byteOffsetWidth = DRAMSim::dramsim_log2((JEDEC_DATA_BUS_BITS / 8));
	unsigned colLowBitWidth = DRAMSim::dramsim_log2(transactionSize) - byteOffsetWidth;
	unsigned colHighBitWidth = colBitWidth - colLowBitWidth;

	switch (DRAMSim::addressMappingScheme) {

		case Scheme1:
			/* {channelID, rankID, rowID, columnID, bankID} */
			logicalAddress = channelID;
			logicalAddress <<= rankBitWidth;
			logicalAddress |= rankID;
			logicalAddress <<= rowBitWidth;
			logicalAddress |= rowID;
			logicalAddress <<= colHighBitWidth;
			logicalAddress |= columnID;
			logicalAddress <<= bankBitWidth;
			logicalAddress |= bankBitWidth;
			logicalAddress <<= colLowBitWidth;
			logicalAddress <<= byteOffsetWidth;
			break;

		case Scheme2:
			/* {channelID, rowID, columnID, bankID, rankID} */
			logicalAddress = channelID;
			logicalAddress <<= rowBitWidth;
			logicalAddress |= rowID;
			logicalAddress <<= colHighBitWidth;
			logicalAddress |= columnID;
			logicalAddress <<= bankBitWidth;
			logicalAddress |= bankID;
			logicalAddress <<= rankBitWidth;
			logicalAddress |= rankID;
			logicalAddress <<= colLowBitWidth;
			logicalAddress <<= byteOffsetWidth;
			break;

		case Scheme3:
			/* {channelID, rankID, bankID, columnID, rowID} */
			logicalAddress = channelID;
			logicalAddress <<= rankBitWidth;
			logicalAddress |= rankID;
			logicalAddress <<= bankBitWidth;
			logicalAddress |= bankID;
			logicalAddress <<= colHighBitWidth;
			logicalAddress |= columnID;
			logicalAddress <<= rowBitWidth;
			logicalAddress |= rowID;
			logicalAddress <<= colLowBitWidth;
			logicalAddress <<= byteOffsetWidth;
			break;

		case Scheme4:
			/* {channelID, rankID, bankID, rowID, columnID} */
			logicalAddress = channelID;
			logicalAddress <<= rankBitWidth;
			logicalAddress |= rankID;
			logicalAddress <<= bankBitWidth;
			logicalAddress |= bankID;
			logicalAddress <<= rowBitWidth;
			logicalAddress |= rowID;
			logicalAddress <<= colHighBitWidth;
			logicalAddress |= columnID;
			logicalAddress <<= colLowBitWidth;
			logicalAddress <<= byteOffsetWidth;
			break;

		case Scheme5:
			/* {channelID, rowID, columnID, rankID, bankID} */
			logicalAddress = channelID;
			logicalAddress <<= rowBitWidth;
			logicalAddress |= rowID;
			logicalAddress <<= colHighBitWidth;
			logicalAddress |= columnID;
			logicalAddress <<= rankBitWidth;
			logicalAddress |= rankID;
			logicalAddress <<= bankBitWidth;
			logicalAddress |= bankID;
			logicalAddress <<= colLowBitWidth;
			logicalAddress <<= byteOffsetWidth;
			break;

		case Scheme6:
			/* {channelID, rowID, bankID, rankID, columnID} */
			logicalAddress = channelID;
			logicalAddress <<= rowBitWidth;
			logicalAddress |= rowID;
			logicalAddress <<= bankBitWidth;
			logicalAddress |= bankID;
			logicalAddress <<= rankBitWidth;
			logicalAddress |= rankID;
			logicalAddress <<= colHighBitWidth;
			logicalAddress |= columnID;
			logicalAddress <<= colLowBitWidth;
			logicalAddress <<= byteOffsetWidth;
			break;

		case Scheme7:
			/* {rowID, columnID, rankID, bankID, channelID} */
			logicalAddress = rowID;
			logicalAddress <<= colHighBitWidth;
			logicalAddress |= columnID;
			logicalAddress <<= rankBitWidth;
			logicalAddress |= rankID;
			logicalAddress <<= bankBitWidth;
			logicalAddress |= bankID;
			logicalAddress <<= channelBitWidth;
			logicalAddress |= channelID;
			logicalAddress <<= colLowBitWidth;
			logicalAddress <<= byteOffsetWidth;
			break;

		default:
			assert(0);
			break;

	}

#ifndef NDEBUG

	unsigned mappedChannelID;
	unsigned mappedRankID;
	unsigned mappedBankID;
	unsigned mappedRowID;
	unsigned mappedColumnID;

	DRAMSim::addressMapping(
			logicalAddress,
			mappedChannelID,
			mappedRankID,
			mappedBankID,
			mappedRowID,
			mappedColumnID);

	assert(mappedChannelID == channelID);
	assert(mappedRankID == rankID);
	assert(mappedBankID == bankID);
	assert(mappedRowID == rowID);
	assert(mappedColumnID == columnID);

#endif

}
