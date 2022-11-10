package main

import (
	"fmt"
	"log"
	"net"

	"gitlab.com/gomidi/midi/v2"

	_ "gitlab.com/gomidi/midi/v2/drivers/portmididrv" // autoregisters driver
)

func main() {
	defer midi.CloseDriver()

	fmt.Println("out ports: \n" + midi.GetOutPorts().String())
	fmt.Println("in ports: \n" + midi.GetInPorts().String())

	out, err := midi.FindOutPort("Teensy MIDI Port 1")
	if err != nil {
		fmt.Printf("can't find teensy keyer")
		return
	}

	in, err := midi.FindInPort("Teensy MIDI Port 1")
	if err != nil {
		fmt.Printf("can't find teensy keyer in port")
		return
	}

	sendMsgFunc, err := midi.SendTo(out)
	if err != nil {
		log.Fatal(err)
	}

	// make wpm to 30wpm
	cwWPMChangemsg := midi.ControlChange(1, 0, 10)
	sendMsgFunc(cwWPMChangemsg)

	_, err = midi.ListenTo(in, recvFunc)
	if err != nil {
		log.Fatal(err)
	}

	getKeyerRespMsg := midi.ControlChange(1, 4, 10)
	sendMsgFunc(getKeyerRespMsg)

	l, err := net.Listen("tcp", "localhost:4004")
	if err != nil {
		log.Fatal(err)
	}

	defer l.Close()

	for {
		conn, err := l.Accept()
		if err != nil {
			log.Fatal(err)
		}

		go handleRequest(conn)
	}
}

func recvFunc(msg midi.Message, ts int32) {
	log.Printf("%v\n", msg)
}

func handleRequest(conn net.Conn) {
	buf := make([]byte, 1024)

	for {
		reqLen, err := conn.Read(buf)
		if err != nil {
			log.Printf("error reading from connection\n")
			return
		}

		if reqLen > 0 {
			fmt.Printf("received command: %s\n", string(buf))
			// buf[0]    - command
			// buf[1...] - command values
			// switch buf[0] {
			// case 0:
			// 	// cw speed change
			// }
		}
	}
}
