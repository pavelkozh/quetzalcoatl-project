import logging
import os
import sys

LOGS_DIR = '/tmp/gaz_logs'

def setup_logger(log_fname):
    if not os.path.exists(LOGS_DIR):
        os.mkdir(LOGS_DIR)

    log_fname = os.path.join(LOGS_DIR, log_fname)
    print('Logging to: {}'.format(log_fname))

    logFormatter = logging.Formatter("%(asctime)s [%(levelname)-5.5s]  %(message)s")
    rootLogger = logging.getLogger()
    rootLogger.setLevel(logging.INFO)

    fileHandler = logging.FileHandler(log_fname)
    fileHandler.setFormatter(logFormatter)
    rootLogger.addHandler(fileHandler)

    consoleHandler = logging.StreamHandler(sys.stdout)
    consoleHandler.setFormatter(logFormatter)
    rootLogger.addHandler(consoleHandler)

def get_logger():
    return logging.getLogger()

def enable_debug():
    logging.getLogger().setLevel(logging.DEBUG)

