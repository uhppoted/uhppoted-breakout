package rpcd

import (
    "fmt"
    "net/rpc"
)

// func (r RPC) GetDoorMode(controller uint32, door uint8) (uint8, error) {
//     debugf("get-door-mode %v %v", controller, door)
// 
//     if door < 1 || door > 4 {
//         return 0, fmt.Errorf("invalid door (%v)", door)
//     } else {
//         var kv = TV{
//             Tag:   fmt.Sprintf("controller.door.%v.mode", door),
//             Value: uint8(mode),
//         }
// 
//         var reply any
// 
//         if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
//             return 0, err
//         } else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
//             return 0, err
//         } else {
//             switch v := reply.(type) {
//             case uint8:
//                 return v, nil
// 
//             case int64:
//                 return uint8(v), nil
// 
//             default:
//                 return 0, fmt.Errorf("unrecognized type (%T)", reply)
//             }
//         }
//     }
// }

// func (r RPC) SetDoorDelay(controller uint32, door uint8, delay uint8) (uint8, error) {
//     debugf("set-door-delay %v %v %v", controller, door, delay)
// 
//     if door < 1 || door > 4 {
//         return 0, fmt.Errorf("invalid door (%v)", door)
//     } else if delay < 1 || delay > 255 {
//         return 0, fmt.Errorf("invalid door delay (%v, %v)", door, delay)
//     } else {
//         var kv = TV{
//             Tag:   fmt.Sprintf("controller.door.%v.delay", door),
//             Value: delay,
//         }
// 
//         var reply any
// 
//         if client, err := rpc.DialHTTP(r.dial.network, r.dial.address); err != nil {
//             return 0, err
//         } else if err := client.Call("RPCD.Set", kv, &reply); err != nil {
//             return 0, err
//         } else {
//             switch v := reply.(type) {
//             case uint8:
//                 return v, nil
// 
//             case int64:
//                 return uint8(v), nil
// 
//             default:
//                 return 0, fmt.Errorf("unrecognized type (%T)", reply)
//             }
//         }
//     }
// }
