import threading, socket, json, time
from game import Game
import assets


def socketConnectThread(name, shouldConnect, oldS):
    s = oldS
    if shouldConnect:
        host = '127.0.0.1'
        port = 4000
        s = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        s.connect((host,port))

    while (assets.sharedInfo['matchType'] == '-'):
        time.sleep(0.2)

    yourMove = -1
    if assets.sharedInfo['matchType'] == 'PvP':
        yourMove = json.dumps({
        'Type': 'GameInit',
        'PlayerColor': 'w',
        'WhoPlays': 'PvP'
                }).encode('utf8')
    else:
        yourMove = json.dumps({
        'Type': 'GameInit',
        'WhoPlays': assets.sharedInfo['matchType'],
        'PlayerColor': 'w',
        'ComputerFirst': assets.sharedInfo['ComputerFirst']
                }).encode('utf8')
    s.send(yourMove)
    
    data = s.recv(1024) # blank doc

    if assets.sharedInfo['ComputerFirst'] is True:
        data = s.recv(1024)
        print('First:')
        print(data)
        y = json.loads(data)
        if 'AI_Move' in y:
            assets.sharedInfo['round'] = y['round']
            assets.sharedInfo['roundBlack'] = y['roundBlack']
            assets.sharedInfo['roundWhite'] = y['roundWhite']
            assets.sharedInfo['whoMoves'] = y['whoMoves']
            assets.sharedInfo['GameOver'] = y['GameOver']
            assets.sharedInfo['AI_Which'] = y['AI_Move']['Which']
            assets.sharedInfo['AI_Where'] = y['AI_Move']['Where']
            assets.sharedInfo['AI_Move'] = True
            assets.sharedInfo['moves'] = y['moves']
            assets.sharedInfo['placements'] = y['placements']
            # assets.sharedInfo['round'] = assets.sharedInfo['round'] + 1
            # assets.sharedInfo['socket'] = False
            # assets.sharedInfo['nextMove'] = True
    else:
        assets.sharedInfo['placements'] = [[0,0]]
        
    assets.sharedInfo['nextMove'] = True
    assets.sharedInfo['socket'] = False
    
    while (assets.sharedInfo['GameOver'] == False):
        while (assets.sharedInfo['socket'] == False):
            time.sleep(0.2)

        # tutaj wysyłanie
        yourMove = {}
        if assets.sharedInfo['messageType'] == 'Move':
            print('wysylam: move')
            yourMove = json.dumps({
                "Type": "Move",
                "Tile": assets.sharedInfo['tileMoved'],
                "Where": assets.sharedInfo['tileMovedWhere']
                    }).encode('utf8')
            s.send(yourMove)
        elif assets.sharedInfo['messageType'] == 'Pass':
            yourMove = json.dumps({
                "Type": "Pass",
                    }).encode('utf8')
            s.send(yourMove)
        elif assets.sharedInfo['messageType'] == 'EndMatch':
            print('wysylam: endmatch')
            assets.sharedInfo['matchType'] = '-'
            assets.matchEnd = True
            yourMove = json.dumps({
                "Type": "EndMatch",
                    }).encode('utf8')
            s.send(yourMove)
            socketConnectThread(False, False, s)
            return
        elif assets.sharedInfo['messageType'] == 'Quit':
            print('wysylam: quit')
            yourMove = json.dumps({
                "Type": "Quit",
                    }).encode('utf8')
            s.send(yourMove)
            return
        #

        data = s.recv(2048)
        print('NOWE DANE:')
        print(data)
        y = json.loads(data)
        assets.sharedInfo['round'] = y['round']
        assets.sharedInfo['roundBlack'] = y['roundBlack']
        assets.sharedInfo['roundWhite'] = y['roundWhite']
        assets.sharedInfo['whoMoves'] = y['whoMoves']
        assets.sharedInfo['GameOver'] = y['GameOver']
        if 'Winner' in y:
            assets.sharedInfo['Winner'] = y['Winner']
        if 'AI_Move' in y:
            assets.sharedInfo['AI_Which'] = y['AI_Move']['Which']
            assets.sharedInfo['AI_Where'] = y['AI_Move']['Where']
            assets.sharedInfo['AI_Move'] = True
        assets.sharedInfo['moves'] = y['moves']
        assets.sharedInfo['placements'] = y['placements']
        # assets.sharedInfo['round'] = assets.sharedInfo['round'] + 1
        assets.sharedInfo['socket'] = False
        assets.sharedInfo['nextMove'] = True
        


if __name__ == '__main__':
    assets.init()
    x = threading.Thread(target=socketConnectThread, args=(1,True, False))
    x.start()

    g = Game()
    g.run()
