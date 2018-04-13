const net = require('net');
const ss = require('./session')

const connections = 999;

let sessions = [];
let sid = 1;
let _creator_tid;

_creator_tid = setInterval(() => {

    let c = net.createConnection(19850, "118.24.48.149", () => {
        sessions[c.sid] = new ss.Session(c);
    });

    c.sid = sid;

    c.on('error', (err) => {
        console.log("connection failed: sid = ", c.sid, err);
    });

    if (sid >= connections) {
        clearInterval(_creator_tid);
        _creator_tid = null;
        console.log("Total", sid, "connection created !");
    } else {
        sid++;
    }

}, 50);