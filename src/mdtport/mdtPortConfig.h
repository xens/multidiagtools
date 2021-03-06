/****************************************************************************
 **
 ** Copyright (C) 2011-2013 Philippe Steinmann.
 **
 ** This file is part of multiDiagTools library.
 **
 ** multiDiagTools is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** multiDiagTools is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with multiDiagTools.  If not, see <http://www.gnu.org/licenses/>.
 **
 ****************************************************************************/
#ifndef MDT_PORT_CONFIG_H
#define MDT_PORT_CONFIG_H

#include <QString>
#include <QByteArray>
#include "mdtFrame.h"

class mdtPortConfig
{
 public:

  mdtPortConfig();
  virtual ~mdtPortConfig();

  /*! \brief Set default configuration
   * 
   * Default configuration is:
   *  - Read and write frame size: 1024
   *  - Read and write queue size: 10
   *  - Use read timeout protocol: false
   *  - Read timout: 5000 [ms]
   *  - Write timout: -1 (infinite)
   *  - Frame type: RAW
   *  - En of frame sequence: LF (ASCII 0x0A)
   *  - Write interframe time: 0 [ms]
   *  - Byte per byte write: Off
   *  - Write interbyte time: 0 [ms]
   *  - Connect timeout: 5000 [ms]
   *  - Connect max try: 10
   */
  virtual void setDefault();

  /*!\brief Set the read frame size
   *
   * Frame size is the maximum number of bytes that can be stored
   * before a frame is full. A full frame that never received
   * a EOF condition is considered invalid
   * \sa mdtFrame
   * \pre size must be a positive value
   */
  void setReadFrameSize(int size);

  /*!\brief Get the read frame size
   * \sa setReadFrameSize()
   */
  int readFrameSize() const;

  /*!\brief Set the read Queue size
   *
   * A queue contain several frames.
   * This parameter gives a limit of the number
   * of frame that can be received before they are threated
   * \pre size must be a positive value
   */
  void setReadQueueSize(int size);

  /*!\brief Get the read Queue size
   * \sa setReadQueueSize()
   */
  int readQueueSize() const;

  /*! \brief Use the read timeout protocol.
   * 
   * An example of read timeout protocol is MODBUS (over serial lines) RTU mode
   * The mdtPortReadThread will use this parameter.
   *
   * \param use If true, read timeout protocol will be used.
   */
  void setUseReadTimeoutProtocol(bool use);

  /*! \brief Know if read timeout protocol must be used
   *
   * \sa setUseReadTimeoutProtocol()
   */
  bool useReadTimeoutProtocol() const;

  /*! \brief Set the read timeout
   * 
   * \param timeout Read timout [ms]
   */
  void setReadTimeout(int timeout);

  /*! \brief Get read timeout
   * 
   * \returns Read timout [ms]
   */
  int readTimeout() const;

  /*! \brief Set the write interframe time
   *
   * If this time is > 0 , the mdtPortWriteThread will
   *  wait defined time before sending a new frame.
   *  This is usefull in timeout based protocols, like Modbus RTU.
   *
   * \param time Interframe time [ms]
   */
  void setWriteInterframeTime(int time);

  /*! \brief Get the write interframe time [ms]
   *
   * \sa setWriteInterframeTime()
   */
  int writeInterframeTime() const;

  /*! \brief Set the write timeout
   *
   * \param timeout Write timout [ms]
   */
  void setWriteTimeout(int timeout);

  /*! \brief Get write timeout
   *
   * \returns Write timout [ms]
   */
  int writeTimeout() const;

  /*!\brief Set the write frame size
   *
   * Frame size is the maximum number of bytes that can be stored
   * before a frame is full.
   *
   * \sa mdtFrame
   * \pre size must be a positive value
   */
  void setWriteFrameSize(int size);

  /*!\brief Get the read frame size
   *
   * \sa setWriteFrameSize()
   */
  int writeFrameSize() const;

  /*!\brief Set the read Queue size
   *
   * A queue contain several frames.
   * This parameter gives a limit of the number
   * of frame that can be enqueued to serial port output.
   *
   * \pre size must be a positive value
   */
  void setWriteQueueSize(int size);

  /*!\brief Get the write Queue size
   *
   * \sa setWriteQueueSize()
   */
  int writeQueueSize() const;

  /*! \brief Set frame type
   *
   * Frame type can be an ASCII frame, binary frame, ...
   *
   * \sa mdtFrame
   */
  void setFrameType(const mdtFrame::type_t type);

  /*! \brief Get frame type
   *
   * \sa setFrameType()
   */
  mdtFrame::type_t frameType() const;

  /*! \brief Set the end of frame sequence (applies on ASCII frames)
   */
  void setEndOfFrameSeq(const QByteArray & seq);

  /*! \brief Set the end of frame sequence (applies on ASCII frames)
   * 
   * This is an overloaded method
   *
   * \sa setEndOfFrameSeq(const QByteArray &)
   */
  void setEndOfFrameSeq(char c);

  /*! \brief Get the end of frame sequence
   * 
   * \sa setEndOfFrameSeq()
   */
  QByteArray endOfFrameSeq() const;

  /*! \brief Set the byte per byte write On/Off
   * 
   * It can happen that a (very) slow device needs time
   * after any received byte.
   *
   * Enable this function in this situation.
   * 
   * \param on True will enable this mode.
   * \param waitTime Wait time between two bytes write [ms]
   * \sa mdtPortWriteThread
   */
  void setBytePerByteWrite(bool on, int waitTime);

  /*! \brief Get byte per byte write flag (On/Off)
   * 
   * \sa setBytePerByteWrite()
   */
  bool bytePerByteWrite() const;

  /*! \brief Get the write time between bytes [ms]
   *
   * \sa setBytePerByteWrite()
   */
  int writeInterbyteTime() const;

  /*! \brief Set the connection timeout
   *
   * \param timeout [ms]
   *
   * This is used by port witch support disconnection
   *  (USB, TCP socket)
   */
  void setConnectTimeout(int timeout);

  /*! \brief Get the connection timeout
   *
   * See setConnectTimeout().
   */
  int connectTimeout() const;

  /*! \brief Set the maximum connection try
   *
   * This is used by port witch support disconnection
   *  (USB, TCP socket)
   */
  void setConnectMaxTry(int maxTry);

  /*! \brief Get the maximum connection try
   *
   * See setConnectMaxTry().
   */
  int connectMaxTry() const;

  bool operator==(const mdtPortConfig &other);
  bool operator!=(const mdtPortConfig &other);

 protected:

  // Frame and frame FIFO (queue) size
  int pvReadFrameSize;            // Maximum data length to store before a frame is considered invalid
  int pvReadQueueSize;            // Maximum number of frames that can be stored
  bool pvUseReadTimeoutProtocol;  // Use the timeout protocol, for example, MODBUS RTU mode. Used by mdtPortReadThread
  int pvReadTimeout;              // Maximum time before reading data [ms]
  int pvWriteFrameSize;           // Maximum data length to store before a frame is considered invalid
  int pvWriteQueueSize;           // Maximum number of frames that can be stored
  int pvWriteInterbyteTime;       // Minimum time  to wait before write call
  int pvWriteInterframeTime;      // Time between each sended frame [ms]
  int pvWriteTimeout;             // Maximum time before port must be ready for writing data [ms]
  mdtFrame::type_t pvFrameType;
  QByteArray pvEndOfFrameSeq;     // End of frame sequence (valid for ASCII frames)
  bool pvBytePerByteWrite;        // For some (very) slow devices that need time between each transmitted byte
  int pvConnectMaxTry;
  int pvConnectTimeout;

  bool matches(const mdtPortConfig &other);
};

#endif  // #ifndef MDT_PORT_CONFIG_H
