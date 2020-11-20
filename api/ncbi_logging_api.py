#! /usr/bin/env python3

import logging, logging.handlers
import os, sys

prev_handler = None

def helper_set_handler( handler ) :
    logger = logging.getLogger()
    this = sys.modules[__name__]
    if this.prev_handler != None :
        this.prev_handler.close()
        logger.removeHandler( this.prev_handler )
    logger.addHandler( handler )
    this.prev_handler = handler

def setup_http_log( host, port ) :
    host_str = '{0}:{1}'.format( host, port )
    handler = logging.handlers.HTTPHandler( host_str, '/', method='GET',
        secure = False )
    helper_set_handler( handler )

class FormatterWithDictionary(logging.Formatter):
    def __init__(self):
        super(FormatterWithDictionary, self).__init__(
            '{ "t":"%(asctime)-15s", "l":"%(levelname)s", "m":"%(message)s", "a":"%(args)s"')

    def format(self, record):
        print( record . extra )
        ret = super().format(record)
        if record.extra:
            ret += ', "extra":"' + str ( record.extra ) + '" }'
        else:
            ret += "}"
        return ret

class myLogger(logging.Logger):
    def __init__( self, root, filename, arg_when, arg_interval ):
        print("myLogger")
        logging.Logger.__init__(self, "")
        self.orig_root = root

        fmt = FormatterWithDictionary()
        handler = logging.handlers.TimedRotatingFileHandler( filename, when=arg_when, interval=arg_interval )
        handler.setFormatter( fmt )

        self.handlers.append(handler)

        # these attributes may have to be kept in sync with the root logger
        # self.name = name
        # self.level = _checkLevel(level)
        # self.parent = None
        # self.propagate = True
        # self.handlers = []
        # self.disabled = False

    def _makeRecord(self, name, level, fn, lno, msg, args, exc_info,
                   func=None, extra=None, sinfo=None):
        print("makeRecord")
        rv = logging.LogRecord(name, level, fn, lno, msg, args, exc_info, func, sinfo)
        rv . extra = extra
        return rv

    def _log(self, level, msg, args, exc_info=None, extra=None, stack_info=False):
        """
        Low-level logging routine which creates a LogRecord and then calls
        all the handlers of this logger to handle the record.
        """
        sinfo = None
        _srcfile = os.path.normcase(logging.addLevelName.__code__.co_filename)
        if _srcfile:
            #IronPython doesn't track Python frames, so findCaller raises an
            #exception on some versions of IronPython. We trap it here so that
            #IronPython can use logging.
            try:
                fn, lno, func, sinfo = self.findCaller(stack_info)
            except ValueError: # pragma: no cover
                fn, lno, func = "(unknown file)", 0, "(unknown function)"
        else: # pragma: no cover
            fn, lno, func = "(unknown file)", 0, "(unknown function)"
        if exc_info:
            if isinstance(exc_info, BaseException):
                exc_info = (type(exc_info), exc_info, exc_info.__traceback__)
            elif not isinstance(exc_info, tuple):
                exc_info = sys.exc_info()
        record = self._makeRecord(self.name, level, fn, lno, msg, args,
                                 exc_info, func, extra, sinfo)
        super().handle(record)

    # the Facade on the root logger

    def makeRecord(self, name, level, fn, lno, msg, args, exc_info,
                   func=None, extra=None, sinfo=None):
        return self.orig_root.makeRecord(name, level, fn, lno, msg, args, exc_info,
                   func, extra, sinfo)

    def setLevel(self, level):
        self.orig_root.setLevel(level)

    def debug(self, msg, *args, **kwargs):
        self.orig_root.debug(msg, *args, **kwargs)
        if self.isEnabledFor(logging.DEBUG):
            self._log(logging.DEBUG, msg, args, **kwargs)

    def info(self, msg, *args, **kwargs):
        self.orig_root.info(msg, *args, **kwargs)
        if self.isEnabledFor(logging.DEBUG):
            self._log(logging.DEBUG, msg, args, **kwargs)

    def warning(self, msg, *args, **kwargs):
        self.orig_root.warning(msg, *args, **kwargs)
        if self.isEnabledFor(logging.WARNING):
            self._log(logging.WARNING, msg, args, **kwargs)

    def warn(self, msg, *args, **kwargs):
        self.orig_root.warn(msg, *args, **kwargs)
        if self.isEnabledFor(logging.WARNING):
            self._log(logging.WARNING, msg, args, **kwargs)

    def error(self, msg, *args, **kwargs):
        self.orig_root.error(msg, *args, **kwargs)
        if self.isEnabledFor(logging.ERROR):
            self._log(logging.ERROR, msg, args, **kwargs)

    def exception(self, msg, *args, exc_info=True, **kwargs):
        self.orig_root.exception(msg, *args, exc_info, **kwargs)
        if self.isEnabledFor(logging.ERROR):
            self._log(logging.ERROR, msg, args, exc_info=exc_info, **kwargs)

    def critical(self, msg, *args, **kwargs):
        self.orig_root.critical(msg, *args, **kwargs)
        if self.isEnabledFor(logging.CRITICAL):
            self._log(logging.CRITICAL, msg, args, **kwargs)

    fatal = critical

    def log(self, level, msg, *args, **kwargs):
        self.orig_root.log(level, msg, *args, **kwargs)
        if self.isEnabledFor(level):
            self._log(level, msg, args, **kwargs)

    def findCaller(self, stack_info=False):
        return self.orig_root.findCaller(stack_info)

    def handle(self, record):
        self.orig_root.handle(record)

    def addHandler(self, hdlr):
        self.orig_root.addHandler(hdlr)

    def removeHandler(self, hdlr):
        self.orig_root.removeHandler(hdlr)

    def hasHandlers(self):
        return self.orig_root.hasHandlers()

    def callHandlers(self, record):
        self.orig_root.callHandlers(record)

    def getEffectiveLevel(self):
        return self.orig_root.getEffectiveLevel()

    def isEnabledFor(self, level):
        return self.orig_root.isEnabledFor(level)

    def getChild(self, suffix):
        return self.orig_root.getChild(suffix)

    def __repr__(self):
        return self.orig_root.__repr__()

def setup_buffer_file_log( filename : str, arg_when = 'M', arg_interval = 1 ):
    logging.root = myLogger(logging.getLogger(), filename, arg_when, arg_interval)
