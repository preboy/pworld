const net = require('net');
const common = require('./common.js');


exports.dispatch = (session, packet) => {
    let opcode = packet.readUInt16LE();
    let message = packet.slice(2);
    console.log("新的消息，opcode=", opcode, "length=", message.byteLength);
    switch (opcode) {
        default: console.log("未知的协议ID:", opcode);
    }
}


exports.make_packet = () => {

    var pid = common.getRandomInt(1, 4000);
    var len = common.getRandomInt(1, 100);
    var buff = Buffer.allocUnsafe(4 + 2 + len);

    buff.writeUInt32LE(2 + len, 0);
    buff.writeUInt16LE(pid, 4);

    for (var i = 6; i < len; i++) {
        var ch = common.getRandomInt(65, 125);
        buff.writeUInt8(ch, i);
    }

    return [buff, pid, len];
}