//
//  custom.swift
//  e-sk8 V2
//
//  Created by Jean LESUR on 07.03.21.
//  Copyright © 2021 Jean LESUR. All rights reserved.
//

import Foundation
import UIKit


class customTableViewCell: UITableViewCell {
    @IBOutlet weak var title: UILabel!
    @IBOutlet weak var titleValue: UILabel!
    @IBOutlet weak var maximumValue: UILabel!
}

class customTableViewController: UITableViewController {
    /*
    override func tableView(_ tableView: UITableView, cellForRowAt indexPath: IndexPath) -> UITableViewCell {
        let cell = tableView.dequeueReusableCell(withIdentifier: "customCell", for: indexPath) as! customTableViewCell
        
        cell.title?.text = infos[indexPath.row]
        cell.titleValue?.text = datas_for_infos[indexPath.row]
        cell.maximumValue?.text = maximas[indexPath.row]
                
        return cell
    }
 */
}
