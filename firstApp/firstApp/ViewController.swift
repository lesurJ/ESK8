//
//  ViewController.swift
//  firstApp
//
//  Created by Jean LESUR on 28/10/2020.
//  Copyright © 2020 Jean LESUR. All rights reserved.
//

import UIKit
import CoreLocation
import CoreBluetooth
import MapKit
import AVFoundation

let AVG_SPEED = 0
let KM = 1
let AUTONOMY = 2
let ALTITUDE = 3
let TEMPERATURE = 4
let TILT = 5


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


let lausanne_coor = CLLocationCoordinate2D(latitude: 46.5333, longitude: 6.6667)
let radius:CLLocationDistance = 1000.0
var myRegion = MKCoordinateRegion(center: lausanne_coor, latitudinalMeters: radius, longitudinalMeters: radius)

var torchState:Bool = false
let list1 = ["Average Speed","Kilometers","Autonomy", "Altitude","Temperature","Temp. FET","Temp. Motor","RPM","Throttle","Input Voltage","Avg Motor Current","RPM"]
var datas = ["1","1","1","1","1","1","1","1","1","1","1","1"]


class ViewController: UIViewController, CLLocationManagerDelegate, UITableViewDelegate, UITableViewDataSource, CBCentralManagerDelegate, CBPeripheralDelegate{
    
    let imageLeftBlinkerOn = UIImage(named: "left_ON")
    let imageLeftBlinkerOff = UIImage(named: "left_OFF")
    let imageRightBlinkerOn = UIImage(named: "right_ON")
    let imageRightBlinkerOff = UIImage(named: "right_OFF")
    let imageRearLightOn = UIImage(named: "headlight_ON")
    let imageRearLightOff = UIImage(named: "headlight_OFF_V2")
    let imageFranceOn = UIImage(named: "france_ON")
    let imageFranceOff = UIImage(named: "france_OFF")
    

    
    //for the MAP
    let locationManager = CLLocationManager()
    var resultSearchController:UISearchController? = nil
    var selectedPin : MKPlacemark? = nil
    
    //for BLE
    var centralManager: CBCentralManager!
    var HM10: CBPeripheral! = nil
    let HM10Name = "HMSoft"
    let HM10ServiceUUID = CBUUID(string: "FFE0")
    var HM10Characteristic: CBCharacteristic! = nil
    var HM10CharacteristicUUID = CBUUID(string: "FFE1")
    var dataString = ""

    
    @IBOutlet weak var label_leds: UILabel!
    @IBOutlet weak var myMap: MKMapView!
    @IBOutlet weak var mySegment: UISegmentedControl!
    @IBOutlet weak var TableView1: UITableView!
    @IBOutlet weak var myScrollView: UIScrollView!
    @IBOutlet weak var torchButton: UIButton!
    @IBOutlet weak var leftBlinker: UIButton!
    @IBOutlet weak var rightBlinker: UIButton!
    @IBOutlet weak var rearLight: UIButton!
    @IBOutlet weak var france: UIButton!
    @IBOutlet weak var connectHM10: UIButton!
    @IBOutlet weak var disconnectHM10: UIButton!
    @IBOutlet weak var statusHM10: UILabel!
    

    override func viewDidLoad() {
        super.viewDidLoad()
        
        locationManager.delegate = self //be able to handle asynchronous requests
        locationManager.requestWhenInUseAuthorization()
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        locationManager.distanceFilter = kCLDistanceFilterNone
        locationManager.startUpdatingLocation()
        locationManager.startUpdatingHeading()
        
        myMap.showsUserLocation = true
        myMap.showsScale = true
        
        TableView1.delegate = self
        TableView1.dataSource = self
        TableView1.layer.cornerRadius = 20
        TableView1.layer.masksToBounds = true
        
        myScrollView.layer.cornerRadius = 20
        myScrollView.layer.masksToBounds = true
        
        torchButton.layer.cornerRadius = 10
        
        //search bar for the map ===========
        let locationSearchTable =  storyboard!.instantiateViewController(withIdentifier: "LocationSearchTable") as! LocationSearchTable
        resultSearchController = UISearchController(searchResultsController: locationSearchTable)
        resultSearchController?.searchResultsUpdater = locationSearchTable
        let searchBar = resultSearchController!.searchBar
        searchBar.sizeToFit()
        searchBar.placeholder = "Search for places"
        navigationItem.titleView = resultSearchController?.searchBar
        resultSearchController?.hidesNavigationBarDuringPresentation = false
        resultSearchController?.dimsBackgroundDuringPresentation = true
        definesPresentationContext = true
        locationSearchTable.myMap = myMap
        locationSearchTable.handleMapSearchDelegate = self
        
        
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    func tableView(_ tableView: UITableView, numberOfRowsInSection section: Int) -> Int {
        return list1.count
    }
    
    func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "cell", for: indexPath)
        cell.textLabel!.text = list1[indexPath.row]
        cell.detailTextLabel!.text = datas[indexPath.row]
        return cell
    }
    
    //toggle the torch on the back of the iphone !
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
    
    @IBAction func selector(_ sender: UISegmentedControl) {
        let pos = sender.selectedSegmentIndex
        print(pos)
        sendCode(code: UInt8(pos))
        //myImage.image = UIImage(named: array[pos])
    }

    @IBAction func mapSegments(_ sender: UISegmentedControl) {
        if sender.selectedSegmentIndex == 0{
            myMap.mapType = .standard
        }
        else{
            myMap.mapType = .satellite
        }
    }
    
    @IBAction func centerButton(_ sender: UIButton) {
        myMap.removeAnnotations(myMap.annotations)
        myMap.setRegion(myRegion, animated: true)
    }
    
    @IBAction func switch_leds(_ sender: UISwitch) {
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
        //sendCode(code: UInt8(selected))
        //datas[0] = String(selected)
        //TableView1.reloadData()
    }
    
    @IBAction func connectHM10(_ sender: UIButton) {
        //print("Connecting HM10...")
        statusHM10.text = "Connecting..."
        centralManager.scanForPeripherals(withServices: nil)
    }
    
    @IBAction func disconnectHM10(_ sender: UIButton) {
        if (HM10 != nil){
            statusHM10.text = "Disconnecting..."
            centralManager.cancelPeripheralConnection(HM10)
        }
    }
    
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
    
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi RSSI: NSNumber) {
        if(peripheral.name == HM10Name){
            //print("DEVICE FOUND")
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
        statusHM10.text = "HM10 disconnected !"
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
                    datas[i] = array[i]
                }
                TableView1.reloadData()
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
            //HM10!.writeValue(Data([code]), for: HM10Characteristic, type: .withoutResponse)
        }
        
    }
    
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        let mUserLocation:CLLocation = locations[0] as CLLocation
        let center = CLLocationCoordinate2D(latitude: mUserLocation.coordinate.latitude, longitude: mUserLocation.coordinate.longitude)
        myRegion = MKCoordinateRegion(center: center, span: MKCoordinateSpan(latitudeDelta: 0.005, longitudeDelta: 0.005))
        
        datas[ALTITUDE] = String(Int(mUserLocation.altitude))
        TableView1.reloadData()
        
    }
}



protocol HandleMapSearch {
    func dropPinZoomIn(placemark: MKPlacemark)
}

extension ViewController : HandleMapSearch{
    func dropPinZoomIn(placemark: MKPlacemark) {
        selectedPin = placemark
        myMap.removeAnnotations(myMap.annotations)
        let annotation = MKPointAnnotation()
        annotation.coordinate = placemark.coordinate
        
        annotation.title = placemark.title
        annotation.subtitle = placemark.locality
        myMap.addAnnotation(annotation)
        
        let span = MKCoordinateSpan(latitudeDelta: 0.05, longitudeDelta: 0.05)
        let region = MKCoordinateRegion(center: placemark.coordinate, span: span)
        myMap.setRegion(region, animated: true)
        
        
    }
}


















// === === === SOME USELESS CODE === === ===
/*
 func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
 
 var identifier : String = ""
 var str:String = ""
 
 switch tableView{
 case firstTableView :
 identifier = "firstCell"
 str = list1[indexPath.row]
 case secondTableView :
 identifier = "secondCell"
 str = list2[indexPath.row]
 default :
 print("This is a default for a switch case")
 }
 
 
 let cell = tableView.dequeueReusableCell(withIdentifier: identifier, for: indexPath)
 cell.textLabel!.text = str
 return cell
 }
 */



/*
 func scrollViewDidEndDecelerating(_ scrollView: UIScrollView) {
 let page_nb = myScrollView.contentOffset.x / myScrollView.frame.size.width
 print(page_nb)
 //myPageControl.currentPage = Int(page_nb)
 }*/
