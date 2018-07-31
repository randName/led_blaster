from flask import Flask, json, request

from .blaster import Blaster

app = Flask(__name__)
app.config.from_envvar('BLASTER_SETTINGS')

try:
    blaster = Blaster(addresses=app.config['ADDRESSES'])
except KeyError:
    blaster = Blaster()


class APIError(Exception):
    pass


@app.errorhandler(APIError)
def api_error(e):
    return json.jsonify({'status': 'error', 'message': str(e)})


@app.route('/', methods=('POST', 'GET'))
def index():
    if request.method == 'GET':
        return app.send_static_file('index.html')

    resp = {'status': 'ok'}
    data = request.get_json()

    try:
        action = data.pop('action')
    except KeyError:
        raise APIError('no action specified')
    except AttributeError:
        raise APIError('no data received')

    data = data.pop('data', {})

    if action == 'nop':
        pass
    elif action == 'sync':
        blaster.t = max(blaster.t) + 1
    elif action == 'update':
        for k, v in data.items():
            blaster[k] = v
    elif action == 'stop':
        blaster.stop(data.get('halt', False))
    else:
        raise APIError('unrecognized action')

    return json.jsonify(resp)
