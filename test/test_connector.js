

let sessions = [];
let sid = 1;

const net = require('net');
const ss  = require('./session')

let err_cnt = 0;

for( let i = 0; i < 1111; i++){

    let c = net.createConnection(7001, "127.0.0.1", () => {
        sessions[sid] = new ss.Session(c, sid);
        sid++;
    });

    c.on('error', (err)=>{
        err_cnt++;
        console.log("err connect count:", err_cnt);
    });
}
