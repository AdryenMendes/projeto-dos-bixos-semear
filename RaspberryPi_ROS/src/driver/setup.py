from setuptools import find_packages, setup

package_name = 'bixos_driver'

setup(
    name=package_name,
    version='0.0.1',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/''/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools', 'pyserial'],
    zip_safe=True,
    maintainer='Seu Nome',
    maintainer_email='seu_email@email.com',
    description='Nó de driver para comunicação com o hardware do robô Bixos.',
    license='Apache-2.0',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'driver_node = bixos_driver.driver_node:main',
        ],
    },
)