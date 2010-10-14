//  Copyright (c) 2009 Miklos Erdelyi
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying 
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef UTILS_SAGA_STREAM_UTILS_HPP_
#define UTILS_SAGA_STREAM_UTILS_HPP_

namespace mapreduce {

// Class for doing timed waits on activities of a saga::stream.
class SagaStreamUtils {
 public:
  // Returns true if the 'stream' is available for the queried activity in
  // 'seconds' time.
  // Actually the operation might take more than 'seconds' seconds.
  static bool TimedWaitForActivity(saga::stream::stream& stream,
                                   saga::stream::activity activity,
                                   int seconds) {
    int n = 0;
    while (n < seconds) {
      saga::stream::activity result = stream.wait(activity, 0.0);
      if (result & activity) {
        break;
      }
      sleep(1);
      ++n;
    }
    return n < seconds;
  }

  static bool TimedWaitForRead(saga::stream::stream& stream, int seconds) {
    return TimedWaitForActivity(stream, saga::stream::Read, seconds);
  }

  static bool TimedWaitForWrite(saga::stream::stream& stream, int seconds) {
    return TimedWaitForActivity(stream, saga::stream::Write, seconds);
  }
};

}  // namespace mapreduce

#endif  // UTILS_SAGA_STREAM_UTILS_HPP_

