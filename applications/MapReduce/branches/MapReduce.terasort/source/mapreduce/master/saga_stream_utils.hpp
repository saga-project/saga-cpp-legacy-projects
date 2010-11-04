bool TimedWaitForRead(saga::stream::stream& stream, int secs) {
  int n = 0;
  while(n < secs) {
    saga::stream::activity res = stream.wait(saga::stream::Read, 0.0);
    bool canread = res & saga::stream::Read;
    //LOG_DEBUG << "res " <<  canread;
    if (res & saga::stream::Read) {
      break;
    }
    sleep(1);
    ++n;
  }
  LOG_DEBUG << "Waited " << n << " times; " << (n < secs) << "\n";
  return n < secs;
}

