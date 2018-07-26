from flask import Flask, json, request

app = Flask(__name__)


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

    if action == 'nop':
        pass
    else:
        raise APIError('unrecognized action')

    return json.jsonify(resp)
