class Config:

    def __init__(self, **kw):
        self.listen = tuple(kw.pop('listen', ('127.0.0.1', 7890)))
        self.verbose = kw.pop('verbose', False)
        self.devices = kw.pop('devices', ())
        self.__dict__.update(kw)

    @staticmethod
    def from_json(fn):
        from json import load as jl
        with open(fn) as f:
            return Config(**jl(f))
