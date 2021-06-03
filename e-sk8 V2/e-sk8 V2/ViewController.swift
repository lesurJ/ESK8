//
//  ViewController.swift
//  e-sk8 V2
//
//  Created by Jean LESUR on 07.03.21.
//  Copyright © 2021 Jean LESUR. All rights reserved.
//

import UIKit
import CoreBluetooth
import CoreLocation
import AVFoundation

//codes to be sent over Bluetooth to the arduino
let INDEX0:UInt8 = 0
let INDEX1:UInt8 = 1
let INDEX2:UInt8 = 2
let INDEX3:UInt8 = 3
let INDEX4:UInt8 = 4
let LEDSON:UInt8 = 5
let LEDSOFF:UInt8 = 6
let LEFTBLINKERON:UInt8 = 7
let LEFTBLINKEROFF:UInt8 = 8
let RIGHTBLINKERON:UInt8 = 9
let RIGHTBLINKEROFF:UInt8 = 10
let REARLIGHTON:UInt8 = 11
let REARLIGHTOFF:UInt8 = 12
let FRANCEON:UInt8 = 13
let FRANCEOFF:UInt8 = 14

//variables for the images
let imageLeftBlinkerOn = UIImage(named: "left_ON")
let imageLeftBlinkerOff = UIImage(named: "left_OFF")
let imageRightBlinkerOn = UIImage(named: "right_ON")
let imageRightBlinkerOff = UIImage(named: "right_OFF")
let imageRearLightOn = UIImage(named: "headlight_ON")
let imageRearLightOff = UIImage(named: "headlight_OFF")
let imageFranceOn = UIImage(named: "france_ON")
let imageFranceOff = UIImage(named: "france_OFF")

var torchState:Bool = false
//data to be fed into the table at the bottom of the app
let infos = ["tempFET","tempMotor","avgMotorCurrent","avgInputCurrent","avgIqCurent","avgIdCurent","Kilometers","dutyCycleNow","rpm","inpVoltage","ampHours","ampHoursCharged","watt_hours","watt_hours_charged","tachometer","tachometerAbs","fault","throttle"]
var datas_for_infos = ["0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0","0.0"]
var maximas = ["1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1","1"]
//colors for the segmented control
let colors = [UIColor .blue, UIColor .red, UIColor .yellow, UIColor .purple, UIColor .gray]

class ViewController: UIViewController, UITableViewDelegate, UITableViewDataSource, CBCentralManagerDelegate, CBPeripheralDelegate, CLLocationManagerDelegate {
    
    let locationManager = CLLocationManager()
    
    //for Bluetooth Low Eenrgy Device (BLE)
    var centralManager: CBCentralManager!
    var HM10: CBPeripheral! = nil
    let HM10Name = "HMSoft"
    let HM10ServiceUUID = CBUUID(string: "FFE0")
    var HM10Characteristic: CBCharacteristic! = nil
    var HM10CharacteristicUUID = CBUUID(string: "FFE1")
    var dataString = ""

    //some outlets to change the properties of the items dynamically
    @IBOutlet weak var connectHM10: UIButton!
    @IBOutlet weak var disconnectHM10: UIButton!
    @IBOutlet weak var statusHM10: UILabel!
    @IBOutlet weak var leftBlinker: UIButton!
    @IBOutlet weak var rightBlinker: UIButton!
    @IBOutlet weak var rearLight: UIButton!
    @IBOutlet weak var france: UIButton!
    @IBOutlet weak var torchButton: UIButton!
    @IBOutlet weak var mySegment: UISegmentedControl!
    @IBOutlet weak var ledSwitch: UISwitch!
    @IBOutlet weak var speedStatus: UILabel!
    
    @IBOutlet weak var viewConnect: UIView!
    @IBOutlet weak var viewSpeed: UIView!
    @IBOutlet weak var viewLights: UIView!
    @IBOutlet weak var tableView: UITableView!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        //instantiate location manager
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        locationManager.requestWhenInUseAuthorization()
        locationManager.startUpdatingLocation()
    
        //do some reshaping of the items. They are nicer now !
        viewConnect.layer.cornerRadius = 20
        viewConnect.layer.masksToBounds = true
        viewSpeed.layer.cornerRadius = 20
        viewSpeed.layer.masksToBounds = true
        viewLights.layer.cornerRadius = 20
        viewLights.layer.masksToBounds = true
        tableView.layer.cornerRadius = 20
        tableView.layer.masksToBounds = true
        tableView.delegate = self
        tableView.dataSource = self
        ledSwitch.onTintColor = UIColor .blue
        
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        speedStatus.text = "\(locations[0].speed*3.6)" //convert to kph
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return infos.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "customCell", for: indexPath) as! customTableViewCell
        //cell.textLabel!.text = infos[indexPath.row]
        //cell.detailTextLabel!.text = datas_for_infos[indexPath.row]
        cell.title?.text = infos[indexPath.row]
        cell.titleValue?.text = datas_for_infos[indexPath.row]
        cell.maximumValue?.text = maximas[indexPath.row]
        
        return cell
    }
    
    
    //to conform with protocol stubs
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        switch central.state {
        case .unknown:
            print("unknown")
        case .resetting:
            print("resetting")
        case .unsupported:
            print("unsupported")
        case .unauthorized:
            print("unauthorized")
        case .poweredOff:
            print("poweredOff")
        case .poweredOn:
            print("poweredOn")
        }
    }
    
    @IBAction func connectHM10(_ sender: UIButton) {
        statusHM10.text = "Connecting..."
        centralManager.scanForPeripherals(withServices: nil)
    }
    
    @IBAction func disconnectHM10(_ sender: UIButton) {
        if (HM10 != nil){
            statusHM10.text = "Disconnecting..."
            centralManager.cancelPeripheralConnection(HM10)
        }
    }
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if(peripheral.name == HM10Name){
            print("DEVICE FOUND")
            statusHM10.text = "HM10 found..."
            HM10 = peripheral
            centralManager.stopScan()
            centralManager.connect(HM10)
        } else {
            print("Not found yet !")
        }
    }
    
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("CONNECTED")
        statusHM10.text = "CONNECTED"
        statusHM10.textColor = UIColor.green
        connectHM10.isEnabled = false
        disconnectHM10.isEnabled = true
        peripheral.delegate = self
        peripheral.discoverServices([HM10ServiceUUID])
    }
    
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Peripheral : \(String(describing: peripheral.name)) has been disconnected !")
        statusHM10.textColor = UIColor.black
        statusHM10.text = "e-sk8 disconnected !"
        connectHM10.isEnabled = true
        disconnectHM10.isEnabled = false
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        print("*******************************************************")
        if ((error) != nil) {
            print("Error discovering services: \(error!.localizedDescription)")
            return
        }
        
        guard let services = peripheral.services else {
            return
        }
        //We need to discover the all characteristic
        for service in services {
            peripheral.discoverCharacteristics(nil, for: service)
            print("Discovered Services: \(services)")
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard let characteristics = service.characteristics else { return }
        
        for characteristic in characteristics {
            HM10Characteristic = characteristic
            print("Discovered Characteristics: \(characteristic)")
            if characteristic.properties.contains(.read) {
                print("\(characteristic.uuid): properties contains .read")
            }
            if characteristic.properties.contains(.notify) {
                print("\(characteristic.uuid): properties contains .notify")
                peripheral.setNotifyValue(true, for: characteristic)
                
            }
        }
    }
    
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        switch characteristic.uuid {
            
        case HM10CharacteristicUUID:
            guard let characteristicData = characteristic.value,
                let stringFromData = String(data: characteristicData, encoding: .utf8) else { return }
            print(stringFromData)
            if stringFromData.contains("<"){//start of message : it is a new message
                dataString = ""
            }
            dataString = dataString + stringFromData
            if stringFromData.contains(">"){
                //print(dataString)
                dataString = dataString.replacingOccurrences(of: "<", with: "")
                dataString = dataString.replacingOccurrences(of: ">", with: "")
                let array = dataString.components(separatedBy: ",")
                for i in 0...array.count-1{
                    datas_for_infos[i] = array[i]
                }
                tableView.reloadData()
            }
            
        default:
            print("Unhandled Characteristic UUID: \(characteristic.uuid)")
        }
    }
    
    func sendCode(code: UInt8){
        print("Sending code : \(code)")
        if(HM10 != nil){
            let str = "<" + String(code) + ">"
            let mydata = str.data(using: String.Encoding.utf8)
            HM10!.writeValue(mydata!, for: HM10Characteristic, type: .withResponse)
        }
    }
    
    @IBAction func toggleTorch(_ sender: UIButton) {
        torchState = !torchState
        guard let device = AVCaptureDevice.default(for: .video) else {return}
        if device.hasTorch{
            do{
                try device.lockForConfiguration()
                if torchState == true {
                    device.torchMode = .on
                } else {
                    device.torchMode = .off
                }
                device.unlockForConfiguration()
            } catch {
                print("Torch could not be used")
            }
        } else {
            print("Torch is unavailable !")
        }
    }
    
    @IBAction func toggleLeftBlinker(_ sender: UIButton) {
        let curr = sender.currentImage
        if (curr == imageLeftBlinkerOn){
            leftBlinker.setImage(imageLeftBlinkerOff, for: UIControl.State.normal)
            sendCode(code: LEFTBLINKEROFF)
        } else if (curr == imageLeftBlinkerOff) {
            leftBlinker.setImage(imageLeftBlinkerOn, for:  UIControl.State.normal)
            sendCode(code: LEFTBLINKERON)
        }
    }
    
    @IBAction func toggleRightBlinker(_ sender: UIButton) {
        let curr = sender.currentImage
        if (curr == imageRightBlinkerOn){
            sender.setImage(imageRightBlinkerOff, for: UIControl.State.normal)
            sendCode(code: RIGHTBLINKEROFF)
        } else if (curr == imageRightBlinkerOff) {
            sender.setImage(imageRightBlinkerOn, for:  UIControl.State.normal)
            sendCode(code: RIGHTBLINKERON)
        }
    }
    
    @IBAction func toggleBackLight(_ sender: UIButton) {
        let curr = sender.currentImage
        if (curr == imageRearLightOn){
            rearLight.setImage(imageRearLightOff, for: UIControl.State.normal)
            sendCode(code: REARLIGHTOFF)
        } else if (curr == imageRearLightOff){
            rearLight.setImage(imageRearLightOn, for: UIControl.State.normal)
            sendCode(code: REARLIGHTON)
        }
    }
    
    @IBAction func toggleFrance(_ sender: UIButton) {
        let curr = sender.currentImage
        if (curr == imageFranceOn){
            sender.setImage(imageFranceOff, for: UIControl.State.normal)
            sendCode(code: FRANCEOFF)
        } else if (curr == imageFranceOff) {
            sender.setImage(imageFranceOn, for:  UIControl.State.normal)
            sendCode(code: FRANCEON)
        }
    }
    
    @IBAction func switchLEDs(_ sender: UISwitch) {
        let value = sender.isOn
        if(value){
            sendCode(code: LEDSON)
        } else {
            sendCode(code: LEDSOFF)
        }
        
        let selected = mySegment.selectedSegmentIndex
        for pos in 0...mySegment.numberOfSegments-1{
            mySegment.setEnabled(value, forSegmentAt: pos)
        }
        mySegment.selectedSegmentIndex = selected
    }
    
    @IBAction func selector(_ sender: UISegmentedControl) {
        let pos = sender.selectedSegmentIndex
        //print(pos)
        ledSwitch.onTintColor = colors[pos]
        sendCode(code: UInt8(pos))
    }
    
}



