// 
//  
//  Copyright (C) 2004-2006  Autodesk, Inc.
//  
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//  
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//  
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//  
#include <string.h>
#include <FdoCommon.h>

FdoIoBufferStream::FdoIoBufferStream( FdoByte* buffer, FdoSize size ) :
    mpBuffer(buffer),
    mbMyBuffer(false),
    mSize(size),
    mLength(size),
    mPos(0)
{
}

FdoIoBufferStream::FdoIoBufferStream( FdoSize size ) :
    mpBuffer(NULL),
    mbMyBuffer(true),
    mLength(0),
    mSize(size),
    mPos(0)
{
    mpBuffer = new FdoByte[size];

}

FdoIoBufferStream::~FdoIoBufferStream(void)
{
    if ( mbMyBuffer && mpBuffer )
        delete[] mpBuffer;
}

FdoIoBufferStream* FdoIoBufferStream::Create( FdoByte* buffer, FdoSize size )
{
    return new FdoIoBufferStream( buffer, size );
}

FdoIoBufferStream* FdoIoBufferStream::Create(FdoSize size )
{
    return new FdoIoBufferStream( size );
}

FdoSize FdoIoBufferStream::Read( FdoByte* buffer, FdoSize count )
{
    // Amount read is lessor of amount to read and bytes after current position.
    FdoSize lRemaining = mLength - mPos;
    FdoSize lCount = (count > lRemaining) ? lRemaining : count;

    memcpy( buffer, (void*) &mpBuffer[mPos], lCount );
    mPos += lCount;

    return lCount;
}

void FdoIoBufferStream::Write( FdoByte* buffer, FdoSize count )
{
    // This simple buffer stream does not dynamically grow.
    if ( (mPos + count) >= mSize )
        throw FdoException::Create(
            FdoException::NLSGetMessage(
                FDO_NLSID(FDO_26_BUFFEROVERWRITEERROR),
                (FdoInt64)count,
                (FdoInt64)mSize - mPos
            )
        );

    memcpy( (void*) &mpBuffer[mPos], buffer, count );
    mPos += count;

    // Update length if wrote past current length.
    mLength = ( mPos > mLength) ? mPos : mLength;
}

void FdoIoBufferStream::Write( FdoIoStream* stream, FdoSize count )
{
    // Amount to write is lesser of bytes remaining in input stream and 
    // given maximum.
    FdoInt64 streamLen = stream->GetLength();
    FdoInt64 lCount = (count > streamLen || count == 0) ? streamLen : count;

    // This simple buffer stream does not dynamically grow.
    FdoInt64 calcSize = mPos + lCount - 1;
	if ( calcSize >= (FdoInt64)mSize )
        throw FdoException::Create(
            FdoException::NLSGetMessage(
                FDO_NLSID(FDO_26_BUFFEROVERWRITEERROR),
                (FdoInt64)lCount,
                (FdoInt64)mSize - mPos
            )
        );

    // When stream is on text file, the number of bytes read can be less than
    // the number requested even when the end of file has not been reached.
    // This happens on Windows since each crlf sequence is converted to one byte
    // (/n).
    // For the above reason, the following loops until no more bytes are read
    // or the requested count is reached.
    while ( lCount > 0 ) {
        FdoInt64 bytesRead = stream->Read( &mpBuffer[mPos], (FdoSize) lCount );
        if ( bytesRead <= 0 ) 
            break;

        mPos += (FdoSize) bytesRead;
        lCount = lCount - bytesRead;
    }

    mLength = ( mPos > mLength) ? mPos : mLength;
}

void FdoIoBufferStream::SetLength( FdoInt64 length )
{

    if ( !mbMyBuffer || (length > mSize ) ) {
        // This simple buffer stream can't increase its size.
        // Also, length cannot be changed if the buffer was passed in.
        throw FdoException::Create(
            FdoException::NLSGetMessage(FDO_NLSID(FDO_27_BUFFERLENGTHERROR))
         );
    }
    else {
        mLength = (length > mLength) ? mLength : (FdoSize) length;
        mPos = ( mPos > mLength ) ? mLength : mPos;
    }

}

void FdoIoBufferStream::Skip( FdoInt64 offset )
{
    FdoInt64 pos = mPos;

    pos += offset;
    pos = ( pos < 0 ) ? 0 : pos;
    pos = ( pos > mLength ) ? mLength : pos;

    mPos = (FdoSize) pos; 
}

void FdoIoBufferStream::Reset()
{
    mPos = 0;
}

