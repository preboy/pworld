

let sessions = [];
let sid = 1;

const net = require('net');
const ss  = require('./session')

for( let i = 0; i < 1; i++){

    let c = net.createConnection(19850, "127.0.0.1", () => {
        sessions[sid] = new ss.Session(c, sid);
        sid++;
    });
}
