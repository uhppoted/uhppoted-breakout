package ssmp

const SYN byte = 22
const ENQ byte = 5
const ACK byte = 6

var SYN_SYN_ACK = []byte{SYN, SYN, ACK}
