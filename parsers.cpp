
int getTrafficFileLineType(string &line) {
  if (line.length() < 4 || line.substr(0, 3) != "sw" + to_string(id)) {
    return T_TYPES.INVALID;
  }
    
  istringstream iss(line);
  vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());

  if (items.size() == 3 && items.at(1) == "delay")
      return T_TYPES.DELAY;
  if (items.size() == 3)
      return T_TYPES.ROUTE;
  return T_TYPES.INVALID;
}

RoutePacket parseTrafficRouteItem(string &line) {
  istringstream iss(line);
  vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());
    
  RoutePacket packet;
  packet.swiID = stoi(items.at(0));
  packet.srcIP = stoi(items.at(1));
  packet.dstIP = stoi(items.at(2));
  return packet;
}

DelayPacket parseTrafficDelayLine(string &line) {
  istringstream iss(line);
  vector<string> items((istream_iterator<string>(iss)), istream_iterator<string>());
    
  DelayPacket packet;
  packet.swiID = stoi(items.at(0));
  packet.delay = stoi(items.at(2));
  return packet;
}