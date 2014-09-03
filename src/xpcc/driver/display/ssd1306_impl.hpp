// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_SSD1306_HPP
#	error	"Don't include this file directly, use 'ssd1306.hpp' instead!"
#endif

template < class I2cMaster >
xpcc::Ssd1306<I2cMaster>::Ssd1306(uint8_t address)
:	I2cDevice<I2cMaster>(), i2cTask(I2cTask::Idle), i2cSuccess(0),
	adapter(address, i2cTask, i2cSuccess), adapterData(address, i2cTask, i2cSuccess)
{
}

// ----------------------------------------------------------------------------
// MARK: - Tasks
template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::ping(void *ctx)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(adapter.configurePing() &&
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter))
	);

	NPT_WAIT_WHILE(i2cTask == I2cTask::Ping);

	if (i2cSuccess == I2cTask::Ping)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::initialize(void *ctx)
{
	NPT_BEGIN(ctx);

	initSuccessful = true;
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetDisplayOff));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetDisplayClockDivideRatio, 0x80));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetMultiplexRatio, 0x3F));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetDisplayOffset, 0x00));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetDisplayStartLine | 0x00));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetChargePump, 0x14));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetMemoryMode, 0x00));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetSegmentRemap127));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetComOutputScanDirectionDecrement));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetComPins, 0x12));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetContrastControl, 0xCE));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetPreChargePeriod, 0xF1));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetV_DeselectLevel, 0x40));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetEntireDisplayResumeToRam));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetNormalDisplay));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetColumnAddress, 0, 127));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetPageAddress, 0, 7));
	initSuccessful &= NPT_SPAWN(writeCommand(ctx, Command::SetDisplayOn));

	if (initSuccessful)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// ----------------------------------------------------------------------------
template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::startWriteDisplay(void *ctx)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(adapterData.configureWrite(buffer, 1024) &&
			(i2cTask = I2cTask::WriteDisplay, this->startTransaction(&adapterData)));

	NPT_END();
}


template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::writeDisplay(void *ctx)
{
	NPT_BEGIN(ctx);

	NPT_SPAWN(startWriteDisplay(ctx));

	NPT_WAIT_WHILE(i2cTask == I2cTask::WriteDisplay);

	if (i2cSuccess == I2cTask::WriteDisplay)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// ----------------------------------------------------------------------------
// MARK: write command
template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					startTransactionWithLength(2) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command, uint8_t data)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data,
					startTransactionWithLength(4) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command, uint8_t data1, uint8_t data2)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data1,
					commandBuffer[4] = 0x80,
					commandBuffer[5] = data2,
					startTransactionWithLength(6) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command,
		uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x00,
					commandBuffer[3] = data1,
					commandBuffer[4] = 0x00,
					commandBuffer[5] = data2,
					commandBuffer[6] = 0x00,
					commandBuffer[7] = data3,
					commandBuffer[8] = 0x00,
					commandBuffer[9] = data4,
					commandBuffer[10] = 0x00,
					commandBuffer[11] = data5,
					startTransactionWithLength(12) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

template < class I2cMaster >
xpcc::pt::Result
xpcc::Ssd1306<I2cMaster>::writeCommand(void *ctx, uint8_t command,
		uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6)
{
	NPT_BEGIN(ctx);

	NPT_WAIT_UNTIL(
			!adapter.isBusy() && (
					commandBuffer[0] = 0x80,
					commandBuffer[1] = command,
					commandBuffer[2] = 0x80,
					commandBuffer[3] = data1,
					commandBuffer[4] = 0x80,
					commandBuffer[5] = data2,
					commandBuffer[6] = 0x80,
					commandBuffer[7] = data3,
					commandBuffer[8] = 0x80,
					commandBuffer[9] = data4,
					commandBuffer[10] = 0x80,
					commandBuffer[11] = data5,
					commandBuffer[12] = 0x80,
					commandBuffer[13] = data6,
					startTransactionWithLength(14) )
	);

	NPT_WAIT_WHILE(i2cTask == command);

	if (i2cSuccess == command)
		NPT_EXIT_SUCCESS();

	NPT_END();
}

// ----------------------------------------------------------------------------
// MARK: start transaction
template < class I2cMaster >
bool
xpcc::Ssd1306<I2cMaster>::startTransactionWithLength(uint8_t length)
{
	return (adapter.configureWrite(commandBuffer, length) &&
			(i2cTask = commandBuffer[1], this->startTransaction(&adapter)));
}
