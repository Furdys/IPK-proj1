# Client-server for Getting Informations about Users (IPK project 1)
Bachelor study at FIT VUT Brno  
4th semester - summer 2018  
Subject: **Computer Communications and Networks (IPK)**

## Score
* Implementation: **4/12**
* Documentation: **7/8**
* Overall: **11/20**
* [Corresponding version](https://github.com/Furdys/IPK-proj1/tree/3393dc4b3324ec0f797b8061c7f72f13bdf533c6)

| Test   | Score |
| ------ | ----- |
| Info   | 0     |
| Folder | 0     |
| List   | 0     |
| NF01   | 0     |
| NF02   | 0     |
| PL01   | 2     |
| PL02   | 2     |

## My commentary
First message (query from client) wasn't expected to be spliced into two or more messages therefore every long query will result in fault.
This is really easily [patched](https://github.com/Furdys/IPK-proj1/commit/a420e3132118a00cc0f8592d6e0c4e92af2c6ff8#diff-aafaa3423d0a1f1dad48ca282ddd666f) with just one more cycle but it wasn't allowed.

## Tests
**Server Start**
```
ipk-server -p 51515
```
**Info (2 points)**
ipk-client -h localhost -p 51515 -n rysavy
ipk-client -h localhost -p 51515 -n biswas
ipk-client -h localhost -p 51515 -n xzwier00
ipk-client -h localhost -p 51515 -n_dhcp
**Folder (2 points)**
ipk-client -h localhost -p 51515 -f rysavy
ipk-client -h localhost -p 51515 -f qpovolny
ipk-client -h localhost -p 51515 -f zizkaj
ipk-client -h localhost -p 51515 -f avahi 

**List (2 points)**
```
ipk-client -h localhost -p 51515 -l rysavy
ipk-client -h localhost -p 51515 -l iegorova
ipk-client -h localhost -p 51515 -l xznebe00
ipk-client -h localhost -p 51515 -l xxx
```
**NF01 (1 points)**
```
ipk-client -h localhost -p 51515 -n r_savy
```
**NF02 (1 points)**
```
ipk-client -h localhost -p 51515 -f rys_vy
```
**PL01 (2 points)**
```
ipk-client -h localhost -p 51515 -l x
```
**PL02 (2 points)**
```
ipk-client -h localhost -p 51515 -l I
```
