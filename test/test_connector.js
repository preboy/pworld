


function make_packet(){

}




let sessions = []


const net = require('net');


for( let i = 0; i < 100; i++){

    let client = net.createConnection("127.0.0.1", 19850, () => {
    
    });

    
    client.on('data', (data) => {

    });


    client.on('end', () => {
        console.log('disconnected from server');
    });


    client.on('drain', ()=> {
        this.sending = false;
    });


    client.on('error', (err)=>{
        console.log("socket errored, sid = ", c._session.sid);
        c.end();
    })

    
    sessions

}

        c.on('data', (data)=>{
			if (this.recv_buffer){
				data = Buffer.concat([this.recv_buffer, data]);
				this.recv_buffer = null;
			}
			if (data.byteLength < 4){
				this.recv_buffer = data;
				return;
			}
			// read header
			let packet_size = data.readUInt32LE(0);
			if (packet_size > 0x10000){
				console.log("超大的包");
				c.end("超大的包");
				return;
			}
			if (data.byteLength-4 < packet_size){
				this.recv_buffer = data;
				return;
			}
			let packet = Buffer.allocUnsafe(packet_size);
			let bytes = data.copy(packet, 0, 4, 4+packet_size);
			if (bytes != packet_size){
				console.log("拷贝数据失败");
				c.end("拷贝数据失败");
				return;
			}
			this.recv_buffer = null;
			if (data.byteLength-4 > packet_size){
				this.recv_buffer = Buffer.allocUnsafe(data.byteLength - (packet_size+4));
				let bytes = data.copy(this.recv_buffer, 0, (packet_size+4), data.byteLength - (packet_size+4));
				if (bytes != data.byteLength-(packet_size+4)){
					console.log("拷贝数据失败2");
					c.end("拷贝数据失败2");
					return;
				}
			}
			dispatcher.dispatch(this, packet);
		})

		c.on('drain', ()=>{
			this.sending = false;
		})

		c.on('error', (err)=>{
			console.log("socket errored, sid = ", c._session.sid);
			c.end();
		})
	}

	send(packet){
		let data = Buffer.allocUnsafe(packet.byteLength+4);
		data.writeUInt32LE(packet.byteLength);
		data.fill(packet, 4);
		if (this.sending){
			this.send_queue.push(data);
			return;
		}
		let send_buffer = data;
		if (this.send_queue.length > 0){
			send_buffer = Buffer.concat(this.recv_buffer);
			send_buffer = Buffer.concat([send_buffer, data]);
			this.recv_buffer = [];
		}
		let ret = this.socket.write(send_buffer);
		if (!ret){
			this.sending = true;
			console.log("data queued in user memory!");
		}
	}

}

