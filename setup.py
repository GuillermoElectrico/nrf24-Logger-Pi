import sys
from setuptools import setup

setup(name='nrf24_logger',
      version=1.1,
      description='Read NRF24 and logger status '+
      'and store in local database.',
      url='https://github.com/GuillermoElectrico/nrf24-Logger-Pi',
      download_url='',
      author='Guillermo',
      platforms='Raspberry Pi',
      classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: MIT License',
        'Operating System :: Raspbian',
        'Programming Language :: Python :: 3.7'
      ],
      keywords='Pi Logger NRF24',
      install_requires=[]+(['setuptools','ez_setup','smbus2', 'influxdb', 'pyyaml'] if "linux" in sys.platform else []),
      license='MIT',
      packages=[],
      include_package_data=True,
      tests_require=[],
      test_suite='',
      zip_safe=True)
