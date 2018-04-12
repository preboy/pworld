const net = require('net');
const ss  = require('./session')


let sessions = [];
let sid = 1;
let err_cnt = 0;


for( let i = 0; i < 111; i++){

    let c = net.createConnection(19850, "118.24.48.149", () => {
        sessions[sid] = new ss.Session(c, sid);
        sid++;
    });

    c.on('error', (err)=>{
        err_cnt++;
        console.log("err connect count:", err_cnt, err);
    });
}
