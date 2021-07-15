//server listen addr
char server_addr_list[5][30]={
    "tcp://*:5555",
    "tcp://*:6666",
    "tcp://*:5050",
    "tcp://*:6060",
    "tcp://*:6565"
};
//client connect addr
//replace the localhost to the actual server addr
//keep the port coinstency with that in server_addr_list
char connect_addr_list[5][30]={
    "tcp://localhost:5555",
    "tcp://localhost:6666",
    "tcp://localhost:5050",
    "tcp://localhost:6060",
    "tcp://localhost:6565"
};
//client subscribe addr
//replace the localhost to the actual server addr
char subaddr[30]="tcp://localhost:7050";
//server publish addr
char pubaddr[30]="tcp://*:7050";